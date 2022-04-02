#include <pch.h>
#include "TrainingProgramFlow.h"
#include "../events/TrainingProgramFlowEvents.h"

namespace Core::Training::Domain
{
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::selectTrainingProgram(uint64_t trainingProgramId, uint16_t numberOfTrainingProgramSteps)
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning)
        {
            // We need to abort the previous program
            resultEvents.push_back(abortCurrentTrainingProgram());
        }

        // Add a new selection event
        auto eventData = std::make_shared<Events::TrainingProgramSelectedEvent>();
        eventData->PreviouslySelectedTrainingProgramId = _selectedTrainingProgramId;
        eventData->SelectedTrainingProgramId = trainingProgramId;
                    
        _selectedTrainingProgramId = trainingProgramId;
        _maxTrainingStepNumber = numberOfTrainingProgramSteps;
        _currentTrainingStepNumber.reset();

        resultEvents.push_back(eventData);

        return resultEvents;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramFlow::unselectTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value())
        {
            if (_selectedTrainingProgramIsRunning)
            {
                // Unselecting a running program will abort it
                resultEvents.push_back(abortCurrentTrainingProgram());
            }

            // Add a reset event no matter if the program was or wasn't running
            auto resetEvent = std::make_shared<Events::TrainingProgramSelectionResetEvent>();
            resetEvent->PreviouslySelectedTrainingProgramId = _selectedTrainingProgramId;
            _selectedTrainingProgramId.reset();
            resultEvents.push_back(resetEvent);
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvents;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::startSelectedTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && !_selectedTrainingProgramIsRunning)
        {
            auto eventData = std::make_shared<Events::TrainingProgramStartedEvent>();
            eventData->TrainingProgramId = _selectedTrainingProgramId.value();

            _selectedTrainingProgramIsRunning = true;

            resultEvent = eventData;
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::activateNextTrainingProgramStep()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;

        if (_selectedTrainingProgramId.has_value() 
            && _selectedTrainingProgramIsRunning 
            && !_runningTrainingProgramIsPaused 
            && (!_currentTrainingStepNumber.has_value() || _currentTrainingStepNumber < _maxTrainingStepNumber - 1))
        {
            if (!_currentTrainingStepNumber.has_value())
            {
                _currentTrainingStepNumber = 0;
            }
            else
            {
                _currentTrainingStepNumber = _currentTrainingStepNumber.value() + 1;
            }

            auto eventData = std::make_shared<Events::TrainingProgramStepActivatedEvent>();
            eventData->TrainingProgramId = _selectedTrainingProgramId.value();
            eventData->TrainingProgramStepNumber = _currentTrainingStepNumber.value();

            resultEvent = eventData;
        }

        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::pauseOrResumeTrainingProgram(bool gameIsPaused, bool trainingProgramIsPaused)
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning)
        {
            if (_runningTrainingProgramIsPaused)
            {
                auto eventData = std::make_shared<Events::TrainingProgramResumedEvent>();
                eventData->TrainingProgramId = _selectedTrainingProgramId.value();
                eventData->GameIsPaused = gameIsPaused;
                eventData->TrainingProgramIsPaused = trainingProgramIsPaused;

                resultEvent = eventData;
            }
            else
            {
                auto eventData = std::make_shared<Events::TrainingProgramPausedEvent>();
                eventData->TrainingProgramId = _selectedTrainingProgramId.value();
                eventData->GameIsPaused = gameIsPaused;
                eventData->TrainingProgramIsPaused = trainingProgramIsPaused;

                resultEvent = eventData;
            }

            _runningTrainingProgramIsPaused = !_runningTrainingProgramIsPaused;            
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::finishRunningTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning && !_runningTrainingProgramIsPaused)
        {
            // Note: This event is supposed to be sent only when a training program finishes naturally by reaching the end of the configured time.
            //       Therefore it must not be paused when stopping it.
            auto eventData = std::make_shared<Events::TrainingProgramFinishedEvent>();
            eventData->TrainingProgramId = _selectedTrainingProgramId.value();

            _selectedTrainingProgramIsRunning = false;
            _currentTrainingStepNumber.reset();

            resultEvent = eventData;
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramFlow::abortRunningTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning)
        {
            resultEvent = abortCurrentTrainingProgram();
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Core::Kernel::DomainEvent> TrainingProgramFlow::abortCurrentTrainingProgram()
    {
        auto abortEvent = std::make_shared<Events::TrainingProgramAbortedEvent>();
        abortEvent->TrainingProgramId = _selectedTrainingProgramId.value();

        _selectedTrainingProgramIsRunning = false;
        _runningTrainingProgramIsPaused = false;
        _currentTrainingStepNumber.reset();

        return abortEvent;
    }
}