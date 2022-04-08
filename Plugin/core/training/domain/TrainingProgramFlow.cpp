#include <pch.h>
#include "TrainingProgramFlow.h"
#include "../events/TrainingProgramFlowEvents.h"

#include <unordered_set>

namespace Core::Training::Domain
{
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::handleTrainingProgramChange(const std::shared_ptr<Configuration::Events::TrainingProgramChangedEvent>& changeEvent)
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> events;

        if ((trainingProgramIsActive()) &&
            _selectedTrainingProgramId.has_value() && _selectedTrainingProgramId.value() == changeEvent->TrainingProgramId)
        {
            // The currently running (or paused) training program has changed => abort it
            events = abortRunningTrainingProgram();
        }
        // Store a copy of the event data (we don't want to take ownership of the original event). Note that operator[] creates an entry if it doesn't exist yet.
        _trainingProgramData[changeEvent->TrainingProgramId] = *changeEvent.get(); 

        return events; // Most often empty
    }

    void TrainingProgramFlow::addStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events) const
    {
        auto stateEvent = std::make_shared<Events::TrainingProgramStateChangedEvent>();
        stateEvent->StartingIsPossible = (_currentTrainingProgramState == Definitions::TrainingProgramState::WaitingForStart);
        stateEvent->StoppingIsPossible = (_currentTrainingProgramState != Definitions::TrainingProgramState::Uninitialized && _currentTrainingProgramState != Definitions::TrainingProgramState::WaitingForStart);
        stateEvent->SwitchingProgramIsPossible = true; // We currently always allow this
        stateEvent->GameIsPaused = _gamePausedState == Definitions::PausedState::Paused;
        stateEvent->PausingProgramIsPossible = _currentTrainingProgramState == Definitions::TrainingProgramState::Running || _currentTrainingProgramState == Definitions::TrainingProgramState::OnlyGamePaused;
        stateEvent->ResumingProgramIsPossible = _currentTrainingProgramState == Definitions::TrainingProgramState::OnlyProgramPaused || _currentTrainingProgramState == Definitions::TrainingProgramState::BothPaused;
        events.push_back(stateEvent);
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::selectTrainingProgram(uint64_t trainingProgramId)
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && trainingProgramIsActive())
        {
            // We need to abort the previous program before switching to a new one
            auto abortEvents = abortCurrentTrainingProgram();
            resultEvents.insert(resultEvents.end(), abortEvents.begin(), abortEvents.end());
        }

        if (_trainingProgramData.count(trainingProgramId) > 0)
        {
            // Add a new selection event
            auto selectionEvent = std::make_shared<Events::TrainingProgramSelectedEvent>();
            selectionEvent->PreviouslySelectedTrainingProgramId = _selectedTrainingProgramId;
            selectionEvent->SelectedTrainingProgramId = trainingProgramId;
            
            const auto& trainingProgramData = _trainingProgramData.at(trainingProgramId);
            selectionEvent->Name = trainingProgramData.TrainingProgramName;
            selectionEvent->Duration = trainingProgramData.TrainingProgramDuration;
            selectionEvent->NumberOfSteps = (uint16_t)trainingProgramData.TrainingProgramEntries.size();

            resultEvents.push_back(selectionEvent);

            // Adapt initial state                    
            _selectedTrainingProgramId = trainingProgramId;
            _currentTrainingStepNumber.reset();
            _currentTrainingProgramState = Definitions::TrainingProgramState::WaitingForStart;

            // Add a state update for the UI
            addStateEvent(resultEvents);
        }

        // Publish the events
        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::unselectTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value())
        {
            if (trainingProgramIsActive())
            {
                // Unselecting a running program will abort it
                auto abortEvents = abortCurrentTrainingProgram();
                resultEvents.insert(resultEvents.end(), abortEvents.begin(), abortEvents.end());
            }

            // Add a reset event no matter if the program was or wasn't running
            auto resetEvent = std::make_shared<Events::TrainingProgramSelectionResetEvent>();
            resetEvent->PreviouslySelectedTrainingProgramId = _selectedTrainingProgramId;
            _selectedTrainingProgramId.reset();
            resultEvents.push_back(resetEvent);

            _currentTrainingProgramState = Definitions::TrainingProgramState::Uninitialized;
        }
        // Else: Rather than throwing an exception, ignore this.

        // Add a state update in any case (we abuse this for an initial update..)
        addStateEvent(resultEvents);

        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::startSelectedTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && !trainingProgramIsActive())
        {
            // This is just a change in state - data are still the same
            _currentTrainingProgramState = Definitions::TrainingProgramState::Running;

            addStateEvent(resultEvents);
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::activateNextTrainingProgramStep()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;

        if (_selectedTrainingProgramId.has_value()
            && _currentTrainingProgramState == Definitions::TrainingProgramState::Running
            && _trainingProgramData.count(_selectedTrainingProgramId.value()) > 0)
        {
            const auto& trainingProgramData = _trainingProgramData.at(_selectedTrainingProgramId.value());

            if (!trainingProgramData.TrainingProgramEntries.empty() &&
                (!_currentTrainingStepNumber.has_value() || _currentTrainingStepNumber < trainingProgramData.TrainingProgramEntries.size() - 1))
            {
                if (!_currentTrainingStepNumber.has_value())
                {
                    _currentTrainingStepNumber = 0;
                }
                else
                {
                    _currentTrainingStepNumber = _currentTrainingStepNumber.value() + 1;
                }
                auto trainingProgramEntry = trainingProgramData.TrainingProgramEntries.at(_currentTrainingStepNumber.value());

                auto stepActivationEvent = std::make_shared<Events::TrainingProgramStepChangedEvent>();
                stepActivationEvent->IsValid = true;
                stepActivationEvent->Name = trainingProgramEntry.Name;
                stepActivationEvent->Duration = trainingProgramEntry.Duration;
                stepActivationEvent->StepNumber = _currentTrainingStepNumber.value();
                resultEvents.push_back(stepActivationEvent);

                // No change in flow state (still running)
            }
        }
        // Return an empty list for any other code path


        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::pauseTrainingProgram()
    {
        _trainingProgramPausedState = Definitions::PausedState::Paused;
        return updatePauseState();
    }
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::resumeTrainingProgram()
    {
        _trainingProgramPausedState = Definitions::PausedState::NotPaused;
        return updatePauseState();
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::handleGamePauseStart()
    {
        _gamePausedState = Definitions::PausedState::Paused;
        return updatePauseState();
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::handleGamePauseEnd()
    {
        _gamePausedState = Definitions::PausedState::NotPaused;
        return updatePauseState();
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::updatePauseState()
    {
        // Find out what is paused and if a state has changed
        const auto trainingProgramIsPaused = _trainingProgramPausedState == Definitions::PausedState::Paused;
        const auto gameIsPaused = _gamePausedState == Definitions::PausedState::Paused;
        const auto trainingProgramPausedStateChanged = 
            gameIsPaused != 
            (_currentTrainingProgramState == Definitions::TrainingProgramState::OnlyProgramPaused || _currentTrainingProgramState == Definitions::TrainingProgramState::BothPaused);
        const auto gamePausedStateChanged =
            trainingProgramIsPaused !=
            (_currentTrainingProgramState == Definitions::TrainingProgramState::OnlyGamePaused || _currentTrainingProgramState == Definitions::TrainingProgramState::BothPaused);

        // Transition through the internal states only when in the Running state or one of the paused states
        // Pausing the game in a different state will send a state update for the UI, but will not transition to a different state
        // Pausing the training program should only be possible in the "Running" and "OnlyGamePaused" states
        if(trainingProgramIsActive())
        {
            if (gameIsPaused && !trainingProgramIsPaused)
            {
                _currentTrainingProgramState = Definitions::TrainingProgramState::OnlyGamePaused;
            }
            else if (!gameIsPaused && trainingProgramIsPaused)
            {
                _currentTrainingProgramState = Definitions::TrainingProgramState::OnlyProgramPaused;
            }
            else if (gameIsPaused && trainingProgramIsPaused)
            {
                _currentTrainingProgramState = Definitions::TrainingProgramState::BothPaused;
            }
            else
            {
                _currentTrainingProgramState = Definitions::TrainingProgramState::Running;
            }
        }

        // Send a state update as soon as a paused flag has changed, even if the internal state is still the same (can e.g. happen when pausing the game in the "WaitingForStart" state).
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (trainingProgramPausedStateChanged || gamePausedStateChanged)
        {
            // At least one state has changed, send an event so at least the UI is updated
            addStateEvent(resultEvents);
        }

        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::finishRunningTrainingProgram()
    {
        // Right now, finishing and aborting a program is the same to us.
        // In the future, we might want to send a different event here so a success message or something can be displayed.

        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && _currentTrainingProgramState == Definitions::TrainingProgramState::Running)
        {
            auto abortEvents = abortCurrentTrainingProgram();
            resultEvents.insert(resultEvents.end(), abortEvents.begin(), abortEvents.end());
        }
        // Else: Rather than throwing an exception, ignore this.

        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::abortRunningTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && trainingProgramIsActive())
        {
            auto abortEvents = abortCurrentTrainingProgram();
            resultEvents.insert(resultEvents.end(), abortEvents.begin(), abortEvents.end());
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvents;
    }

    std::vector<Configuration::Domain::TrainingProgramEntry> TrainingProgramFlow::getCurrentEntries()
    {
        std::vector<Configuration::Domain::TrainingProgramEntry> entries;
        if (_selectedTrainingProgramId.has_value() && _trainingProgramData.count(_selectedTrainingProgramId.value() > 0))
        {
            entries = _trainingProgramData.at(_selectedTrainingProgramId.value()).TrainingProgramEntries;
        }
        return entries;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::createTimeUpdatedEvent(const std::chrono::milliseconds& passedTime, const std::chrono::milliseconds& nextThreshold)
    {
        std::shared_ptr<Kernel::DomainEvent> updateEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _trainingProgramData.count(_selectedTrainingProgramId.value() > 0))
        {
            const auto& currentTrainingProgram = _trainingProgramData.at(_selectedTrainingProgramId.value());
            if (_currentTrainingStepNumber.has_value() && _currentTrainingStepNumber.value() < currentTrainingProgram.TrainingProgramEntries.size())
            {
                const auto& currentTrainingStep = currentTrainingProgram.TrainingProgramEntries.at(_currentTrainingStepNumber.value());

                auto timeUpdatedEvent = std::make_shared<Events::TrainingTimeUpdatedEvent>();

                timeUpdatedEvent->TotalTrainingDuration = currentTrainingProgram.TrainingProgramDuration;
                timeUpdatedEvent->TimeSpentInTraining = passedTime;
                timeUpdatedEvent->CurrentTrainingStepDuration = currentTrainingStep.Duration;
                timeUpdatedEvent->TimeLeftInProgram = currentTrainingProgram.TrainingProgramDuration - passedTime;
                timeUpdatedEvent->TimeLeftInCurrentTrainingStep = nextThreshold - timeUpdatedEvent->TimeSpentInTraining;

                // Fixup negative values (happens when "passedTime" doesn't match the training program duration exactly, which might be always)
                if (timeUpdatedEvent->TimeLeftInProgram.count() < 0) 
                {
                    timeUpdatedEvent->TimeLeftInProgram = std::chrono::milliseconds(0);
                }
                if (timeUpdatedEvent->TimeLeftInCurrentTrainingStep.count() < 0)
                {
                    timeUpdatedEvent->TimeLeftInCurrentTrainingStep = std::chrono::milliseconds(0);
                }

                updateEvent = timeUpdatedEvent;
            }
        }
        return updateEvent;

    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::abortCurrentTrainingProgram()
    {
        // No matter if a training program gets aborted or finished, we reset to the WaitingForStart state.
        // We also clear the current training step since none is active
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;

        auto stepActivationEvent = std::make_shared<Events::TrainingProgramStepChangedEvent>();
        stepActivationEvent->IsValid = false;
        resultEvents.push_back(stepActivationEvent);

        _currentTrainingProgramState = Definitions::TrainingProgramState::WaitingForStart;
        _trainingProgramPausedState = Definitions::PausedState::NotPaused;
        _currentTrainingStepNumber.reset();

        addStateEvent(resultEvents);

        return resultEvents;
    }
    bool TrainingProgramFlow::trainingProgramIsActive() const
    {
        // The program is "active" if it is either running or paused
        auto relevantStates = std::unordered_set<Definitions::TrainingProgramState>{
            Definitions::TrainingProgramState::Running,
            Definitions::TrainingProgramState::OnlyGamePaused,
            Definitions::TrainingProgramState::OnlyProgramPaused,
            Definitions::TrainingProgramState::BothPaused
        };
        return relevantStates.count(_currentTrainingProgramState) > 0;
    }
}