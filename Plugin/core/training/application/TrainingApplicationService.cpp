#include <pch.h>

#include "TrainingApplicationService.h"
#include "../events/TrainingProgramFlowEvents.h"
#include <core/kernel/InvalidStateException.h>

namespace Core::Training::Application
{
	TrainingApplicationService::TrainingApplicationService()
	{
		updateReadModels(_trainingProgramFlow->unselectTrainingProgram()); // Will generate just a state vent
	}

	void TrainingApplicationService::selectTrainingProgram(const Commands::SelectTrainingProgramCommand& command)
	{
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

		LOG("Starting");
		_referenceTime = std::chrono::steady_clock::now();

		updateReadModels(events);
	}

	void TrainingApplicationService::pauseTrainingProgram(const Commands::PauseTrainingProgramCommand&)
	{
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Running &&
			_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::OnlyGamePaused)
		{
			throw Kernel::InvalidStateException(
				"You tried pausing a training program, even though no training program was running, or the program was paused already."
			);
		}

		LOG("Pausing");

		updateReadModels(_trainingProgramFlow->pauseTrainingProgram());
		handlePauseChange();
	}

	void TrainingApplicationService::resumeTrainingProgram(const Commands::ResumeTrainingProgramCommand&)
	{
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::OnlyProgramPaused &&
			_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::BothPaused)
		{
			throw Kernel::InvalidStateException(
				"You tried resuming a training program which was not paused."
			);
		}

		LOG("Resuming");

		updateReadModels(_trainingProgramFlow->resumeTrainingProgram());
		handlePauseChange();
	}

	void TrainingApplicationService::abortTrainingProgram(const Commands::AbortTrainingProgramCommand&)
	{
		LOG("Aborting");
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
		handlePauseChange();
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
				LOG("Finishing");
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
				LOG("Activating next step");
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

	void TrainingApplicationService::handlePauseChange()
	{
		if (!_trainingProgramFlow->trainingProgramIsActive())
		{
			// We're not interested in pause changes when there is no program running
			return;
		}
		if (_trainingProgramFlow->currentTrainingProgramState() != Definitions::TrainingProgramState::Running)
		{
			// Remember the start of the pause so we can remove the pause time from the training time
			if (!_pauseStartTime.has_value())
			{
				LOG("Starting pause");
				_pauseStartTime = std::chrono::steady_clock::now();
			}
			// else: Ignore this, since e.g. the game was paused after the training program had already been paused
		}
		else
		{
			// Shift the start of the training program as if the pause happened before it. That way, we can always compare to the reference time
			if (_pauseStartTime.has_value())
			{
				LOG("Finished pause");
				_referenceTime = std::chrono::steady_clock::now() - (_pauseStartTime.value() - _referenceTime);
				_pauseStartTime.reset();
			}
		}
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
		// Currently, we're not interested in any other event, but the read mdoels might be

		updateReadModels({ genericEvent });
	}
}
