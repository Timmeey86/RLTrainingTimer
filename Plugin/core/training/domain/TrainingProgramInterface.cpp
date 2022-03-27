#include <pch.h>
#include "TrainingProgramInterface.h"
#include "../events/TrainingProgramInterfaceEvents.h"

namespace Core::Training::Domain
{
    std::shared_ptr<Kernel::DomainEvent> TrainingProgramInterface::selectTrainingProgram(uint64_t trainingProgramId)
    {
        auto eventData = std::make_shared<Events::TrainingProgramSelectedEvent>();
        eventData->PreviouslySelectedTrainingProgramId = _selectedTrainingProgramId;
        eventData->SelectedTrainingProgramId = trainingProgramId;
                    
        _selectedTrainingProgramId = trainingProgramId;

        return eventData;
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramInterface::unselectTrainingProgram()
    {
        std::vector<std::shared_ptr<Kernel::DomainEvent>> resultEvents;
        if (_selectedTrainingProgramId.has_value())
        {
            if (_selectedTrainingProgramIsRunning)
            {
                // Unselecting a running program will abort it
                auto abortEvent = std::make_shared<Events::TrainingProgramAbortedEvent>();
                abortEvent->TrainingProgramId = _selectedTrainingProgramId.value();
                resultEvents.push_back(abortEvent);

                _selectedTrainingProgramIsRunning = false;
                _runningTrainingProgramIsPaused = false;
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

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramInterface::startSelectedTrainingProgram()
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

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramInterface::pauseOrResumeTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning)
        {
            if (_runningTrainingProgramIsPaused)
            {
                auto eventData = std::make_shared<Events::TrainingProgramResumedEvent>();
                eventData->TrainingProgramId = _selectedTrainingProgramId.value();

                resultEvent = eventData;
            }
            else
            {
                auto eventData = std::make_shared<Events::TrainingProgramPausedEvent>();
                eventData->TrainingProgramId = _selectedTrainingProgramId.value();

                resultEvent = eventData;
            }

            _runningTrainingProgramIsPaused = !_runningTrainingProgramIsPaused;            
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramInterface::finishRunningTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning && !_runningTrainingProgramIsPaused)
        {
            // Note: This event is supposed to be sent only when a training program finishes naturally by reaching the end of the configured time.
            //       Therefore it must not be paused when stopping it.
            auto eventData = std::make_shared<Events::TrainingProgramFinishedEvent>();
            eventData->TrainingProgramId = _selectedTrainingProgramId.value();

            _selectedTrainingProgramIsRunning = false;

            resultEvent = eventData;
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramInterface::abortRunningTrainingProgram()
    {
        std::shared_ptr<Kernel::DomainEvent> resultEvent = nullptr;
        if (_selectedTrainingProgramId.has_value() && _selectedTrainingProgramIsRunning)
        {
            auto eventData = std::make_shared<Events::TrainingProgramAbortedEvent>();
            eventData->TrainingProgramId = _selectedTrainingProgramId.value();

            _selectedTrainingProgramIsRunning = false;
            _runningTrainingProgramIsPaused = false;

            resultEvent = eventData;
        }
        // Else: Rather than throwing an exception, ignore this.
        return resultEvent;
    }
}