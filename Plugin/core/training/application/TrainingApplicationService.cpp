#include <pch.h>

#include "TrainingApplicationService.h"
#include "../events/TrainingProgramFlowEvents.h"
#include <core/kernel/InvalidStateException.h>

namespace Core::Training::Application
{
	TrainingApplicationService::TrainingApplicationService() = default;

	void TrainingApplicationService::selectTrainingProgram(const Commands::SelectTrainingProgramCommand& command)
	{
		if(_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Uninitialized &&
			_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::WaitingForStart)
		{
			throw Kernel::InvalidStateException(
				"You tried selecting a training program while a different program was already running. You need to stop the running training program first."
			);
		}
		std::vector<std::shared_ptr<Kernel::DomainEvent>> events;
		if (command.TrainingProgramId.has_value())
		{
			auto selectionEvents = _trainingProgramFlow->selectTrainingProgram(command.TrainingProgramId.value());
			events.insert(events.end(), selectionEvents.begin(), selectionEvents.end());
		}
		else
		{
			auto selectionEvents = _trainingProgramFlow->unselectTrainingProgram();
			events.insert(events.end(), selectionEvents.begin(), selectionEvents.end());
		}
		updateReadModels(events);
	}
	void TrainingApplicationService::startTrainingProgram(const Commands::StartTrainingProgramCommand&)
	{
		if(_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::WaitingForStart)
		{
			throw Kernel::InvalidStateException(
				"You tried starting a training program while there was either no program selected, or a different program was still running."
			);
		}
		// Precalculate program steps
		auto trainingProgramEntryEndTime = std::chrono::milliseconds(0);
		_trainingProgramEntryEndTimes.clear();
		const auto& entries = _trainingProgramFlow->getCurrentEntries();
		for (const auto& trainingProgramEntry : entries)
		{
			trainingProgramEntryEndTime += std::chrono::milliseconds(trainingProgramEntry.Duration);
			_trainingProgramEntryEndTimes.push_back(trainingProgramEntryEndTime);
		}
		
		std::vector<std::shared_ptr<Kernel::DomainEvent>> events;
		// Move to the "Running" state
		auto selectionEvents = _trainingProgramFlow->startSelectedTrainingProgram();
		// Activate the first training step
		auto nextStepEvents = _trainingProgramFlow->activateNextTrainingProgramStep();
		// Pause the training program right away in case the game is currently paused
		auto pauseEvents = _trainingProgramFlow->updatePauseState();

		events.insert(events.end(), selectionEvents.begin(), selectionEvents.end());
		events.insert(events.end(), nextStepEvents.begin(), nextStepEvents.end());
		events.insert(events.end(), pauseEvents.begin(), pauseEvents.end());

		updateReadModels(events);
	}

	void TrainingApplicationService::pauseTrainingProgram(const Commands::PauseTrainingProgramCommand&)
	{
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Running)
		{
			throw Kernel::InvalidStateException(
				"You tried pausing a training program, even though no training program was running, or the program was paused already."
			);
		}
		updateReadModels(_trainingProgramFlow->pauseTrainingProgram());
	}

	void TrainingApplicationService::resumeTrainingProgram(const Commands::ResumeTrainingProgramCommand&)
	{
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Paused)
		{
			throw Kernel::InvalidStateException(
				"You tried resuming a training program which was not paused."
			);
		}
		updateReadModels(_trainingProgramFlow->resumeTrainingProgram());
	}

	void TrainingApplicationService::abortTrainingProgram(const Commands::AbortTrainingProgramCommand&)
	{
		// Aborting is allowed from any state
		updateReadModels(_trainingProgramFlow->abortRunningTrainingProgram());
	}
	void TrainingApplicationService::setGameState(Definitions::PausedState gameState)
	{
		std::vector<std::shared_ptr<Kernel::DomainEvent>> events;
		if (gameState == Definitions::PausedState::Paused)
		{
			events = _trainingProgramFlow->handleGamePauseStart();
		}
		else
		{
			events = _trainingProgramFlow->handleGamePauseEnd();
		}
		updateReadModels(events);
	}
	void TrainingApplicationService::processTimerTick()
	{
		// This would usually be in a domain service most likely
		
		// Note: This method will be called _very_ often. Keep actions in here to a minimum
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Running)
		{
			return; // The "running" state is the only one in which we will ever need to make time comparisons
		}
		auto currentTrainingProgramEntryIndex = _trainingProgramFlow->currentTrainingStepNumber().value();
		// Note: We assume that the _trainingProgramEntryEndTimes are matching the training program while in the running state, for the sake of performance
		auto nextThreshold = _trainingProgramEntryEndTimes[currentTrainingProgramEntryIndex];

		auto passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _referenceTime);

		std::vector<std::shared_ptr<Kernel::DomainEvent>> events;
		if (passedTime > nextThreshold)
		{
			if (currentTrainingProgramEntryIndex == _trainingProgramEntryEndTimes.size() - 1)
			{
				// Training has finished
				auto finishEvents = _trainingProgramFlow->finishRunningTrainingProgram();
				events.insert(events.end(), finishEvents.begin(), finishEvents.end());
				
				if (auto updateEvent = _trainingProgramFlow->createTimeUpdatedEvent(nextThreshold, nextThreshold);
					updateEvent != nullptr)
				{
					events.push_back(updateEvent);
				}
			}
			else
			{
				// The end of the current step has been reached, and there is another one
				
				// Instead of a time update event, only switch to the next step (otherwise we would have to repeat what we did in the beginning of this method, for the new step index)
				// There will be a timer tick almost immediately after this one anyway
				auto nextStepEvents = _trainingProgramFlow->activateNextTrainingProgramStep();
				events.insert(events.end(), nextStepEvents.begin(), nextStepEvents.end());
			}
		}
		else
		{
			if (auto updateEvent = _trainingProgramFlow->createTimeUpdatedEvent(passedTime, nextThreshold);
				updateEvent != nullptr)
			{
				events.push_back(updateEvent);
			}
		}
		updateReadModels(events);
	}

	void TrainingApplicationService::updateReadModels(std::vector<std::shared_ptr<Kernel::DomainEvent>> genericEvents)
	{
		for (const auto& genericEvent : genericEvents)
		{
			if (genericEvent == nullptr) { continue; }

			_displayReadModel.dispatchEvent(genericEvent);
			_flowControlReadModel.dispatchEvent(genericEvent);
		}
	}

	void TrainingApplicationService::processEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent)
	{
		if (auto programChangeEvent = std::dynamic_pointer_cast<Configuration::Events::TrainingProgramChangedEvent>(genericEvent);
			programChangeEvent != nullptr)
		{
			_trainingProgramFlow->handleTrainingProgramChange(programChangeEvent);
		}
		// Currently, we're not interested in any other event
	}
}
