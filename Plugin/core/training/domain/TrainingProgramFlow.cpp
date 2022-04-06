#include <pch.h>
#include "TrainingProgramFlow.h"
#include "../events/TrainingProgramFlowEvents.h"

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

    void addUninitializedStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        auto stateEvent = std::make_shared<Events::TrainingProgramStateChangedEvent>();
        stateEvent->StartingIsPossible = false;
        stateEvent->PausingIsPossible = false;
        stateEvent->ResumingIsPossible = false;
        stateEvent->StoppingIsPossible = false;
        stateEvent->SwitchingProgramIsPossible = true;
        events.push_back(stateEvent);
    }

    void addWaitingForStartStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        auto stateEvent = std::make_shared<Events::TrainingProgramStateChangedEvent>();
        stateEvent->StartingIsPossible = true;
        stateEvent->PausingIsPossible = false;
        stateEvent->ResumingIsPossible = false;
        stateEvent->StoppingIsPossible = false;
        stateEvent->SwitchingProgramIsPossible = true;
        events.push_back(stateEvent);
    }

    void addRunningStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        auto stateEvent = std::make_shared<Events::TrainingProgramStateChangedEvent>();
        stateEvent->StartingIsPossible = false;
        stateEvent->PausingIsPossible = true;
        stateEvent->ResumingIsPossible = false;
        stateEvent->StoppingIsPossible = true;
        stateEvent->SwitchingProgramIsPossible = true; // TODO: See if we can get this to work. Otherwise disable in all states except for waiting for start and uninitialized
        events.push_back(stateEvent);
    }

    void addPausedStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        auto stateEvent = std::make_shared<Events::TrainingProgramStateChangedEvent>();
        stateEvent->StartingIsPossible = false;
        stateEvent->PausingIsPossible = false;
        stateEvent->ResumingIsPossible = true;
        stateEvent->StoppingIsPossible = true;
        stateEvent->SwitchingProgramIsPossible = true;
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

            // Add a state update for the UI
            addWaitingForStartStateEvent(resultEvents);

            // Adapt initial state                    
            _selectedTrainingProgramId = trainingProgramId;
            _currentTrainingStepNumber.reset();
            _currentTrainingProgramState = Definitions::TrainingProgramState::WaitingForStart;
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

            // Add a state update
            addUninitializedStateEvent(resultEvents);

            _currentTrainingProgramState = Definitions::TrainingProgramState::Uninitialized;
        }
        // Else: Rather than throwing an exception, ignore this.

        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::startSelectedTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && !trainingProgramIsActive())
        {
            // This is just a change in state - data are still the same
            addRunningStateEvent(resultEvents);

            _currentTrainingProgramState = Definitions::TrainingProgramState::Running;
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

            if (!_currentTrainingStepNumber.has_value() || _currentTrainingStepNumber < trainingProgramData.TrainingProgramEntries.size() - 1)
            {
                if (!_currentTrainingStepNumber.has_value())
                {
                    _currentTrainingStepNumber = 0;
                }
                else
                {
                    _currentTrainingStepNumber = _currentTrainingStepNumber.value() + 1;
                }
                auto stepActivationEvent = std::make_shared<Events::TrainingProgramStepChangedEvent>();
                stepActivationEvent->IsValid = true;
                stepActivationEvent->Name = trainingProgramData.TrainingProgramName;
                stepActivationEvent->Duration = trainingProgramData.TrainingProgramDuration;
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
        auto flowIsPaused = (_currentTrainingProgramState == Definitions::TrainingProgramState::Paused);
        auto flowShouldBePaused = _gamePausedState == Definitions::PausedState::Paused || _trainingProgramPausedState == Definitions::PausedState::Paused;

        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (!flowIsPaused && flowShouldBePaused && _currentTrainingProgramState == Definitions::TrainingProgramState::Running)
        {
            addPausedStateEvent(resultEvents);
            _currentTrainingProgramState = Definitions::TrainingProgramState::Paused;
        }
        if (flowIsPaused && !flowShouldBePaused && _currentTrainingProgramState == Definitions::TrainingProgramState::Paused)
        {
            addRunningStateEvent(resultEvents);
            _currentTrainingProgramState = Definitions::TrainingProgramState::Running;
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
                timeUpdatedEvent->TimeLeftInCurrentTrainingStep = nextThreshold - timeUpdatedEvent->CurrentTrainingStepDuration;

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

        addWaitingForStartStateEvent(resultEvents);

        _currentTrainingProgramState = Definitions::TrainingProgramState::WaitingForStart;
        _trainingProgramPausedState = Definitions::PausedState::NotPaused;
        _currentTrainingStepNumber.reset();

        return resultEvents;
    }
    bool TrainingProgramFlow::trainingProgramIsActive() const
    {
        return _currentTrainingProgramState == Definitions::TrainingProgramState::Running || _currentTrainingProgramState == Definitions::TrainingProgramState::Paused;
    }
}