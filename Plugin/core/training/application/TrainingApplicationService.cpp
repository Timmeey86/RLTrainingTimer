#include <pch.h>

#include "TrainingApplicationService.h"
#include "../events/TrainingProgramFlowEvents.h"
#include <core/kernel/InvalidStateException.h>

namespace Core::Training::Application
{
	TrainingApplicationService::TrainingApplicationService(
		std::shared_ptr<Configuration::Domain::TrainingProgramList> trainingProgramList,
		std::shared_ptr<Domain::TrainingProgramFlow> trainingProgramFlow)
		: _trainingProgramList{ trainingProgramList }
		, _trainingProgramFlow{ trainingProgramFlow }
	{
		// TODO: Update this whenever the list changes
		_readModel.updateTrainingProgramListEntries(_trainingProgramList->getListEntries());
	}
	void TrainingApplicationService::selectTrainingProgram(const Commands::SelectTrainingProgramCommand& command)
	{
		if (_trainingProgramState != TrainingProgramState::Uninitialized && _trainingProgramState != TrainingProgramState::WaitingForStart)
		{
			throw Kernel::InvalidStateException(
				"You tried selecting a training program while a different program was already running. You need to stop the running training program first."
			);
		}
		_currentTrainingProgram = nullptr;
		std::vector<std::shared_ptr<Kernel::DomainEvent>> events;
		if (command.TrainingProgramId.has_value())
		{
			// Retrieve the training program (will throw if the ID is wrong)
			_currentTrainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId.value());
			events = _trainingProgramFlow->selectTrainingProgram(_currentTrainingProgram->id(), (uint16_t)_currentTrainingProgram->entries().size());
			_trainingProgramState = TrainingProgramState::WaitingForStart;
		}
		else
		{
			_trainingProgramState = TrainingProgramState::Uninitialized;
			events = _trainingProgramFlow->unselectTrainingProgram();
		}
		updateReadModel(events);
	}
	void TrainingApplicationService::startTrainingProgram(const Commands::StartTrainingProgramCommand& command)
	{
		if (_trainingProgramState != TrainingProgramState::WaitingForStart)
		{
			throw Kernel::InvalidStateException(
				"You tried starting a training program while there was either no program selected, or a different program was still running."
			);
		}
		// Precalculate program steps
		auto trainingProgramEntryEndTime = std::chrono::milliseconds(0);
		_trainingProgramEntryEndTimes.clear();
		for (const auto& trainingProgramEntry : _currentTrainingProgram->entries())
		{
			trainingProgramEntryEndTime += std::chrono::milliseconds(trainingProgramEntry.duration());
			_trainingProgramEntryEndTimes.push_back(trainingProgramEntryEndTime);
		}

		auto selectionEvent = _trainingProgramFlow->startSelectedTrainingProgram();
		auto firstStepActivationEvent = _trainingProgramFlow->activateNextTrainingProgramStep();
		// Since training program flow currently does not know about the actual training program except for its ID, we need to add durations here
		// TODO: It is probably better to just have TrainingProgramFlow know the program and then send event details itself.		
		if (auto castedActivationEvent = dynamic_cast<Events::TrainingProgramStepActivatedEvent*>(firstStepActivationEvent.get()); 
			castedActivationEvent != nullptr)
		{
			auto trainingProgramStep = _currentTrainingProgram->entries().at(castedActivationEvent->TrainingProgramStepNumber);
			castedActivationEvent->TrainingProgramStepName = trainingProgramStep.name();
			castedActivationEvent->TrainingProgramStepDuration = trainingProgramStep.duration();
		}
		_referenceTime = std::chrono::steady_clock::now();
		_trainingProgramState = TrainingProgramState::Running;
		updateReadModel({ selectionEvent, firstStepActivationEvent });

		// Handle a pause change just in case the ingame pause menu is open while the user starts the training program (not unlikely)
		handlePauseChange();
	}
	void TrainingApplicationService::pauseTrainingProgram(const Commands::PauseTrainingProgramCommand& command)
	{
		if (_trainingProgramState != TrainingProgramState::Running)
		{
			throw Kernel::InvalidStateException(
				"You tried pausing a training program, even though no training program was running, or the program was paused already."
			);
		}
		_trainingProgramState = TrainingProgramState::Paused;
		handlePauseChange();
	}
	void TrainingApplicationService::resumeTrainingProgram(const Commands::ResumeTrainingProgramCommand& command)
	{
		if (_trainingProgramState != TrainingProgramState::Paused)
		{
			throw Kernel::InvalidStateException(
				"You tried resuming a training program which was not paused."
			);
		}
		_trainingProgramState = TrainingProgramState::Running;
		handlePauseChange();
	}
	void TrainingApplicationService::abortTrainingProgram(const Commands::AbortTrainingProgramCommand& command)
	{
		// TODO: Make sure abort is called whenever the user edits the active training program (they can edit other programs without issues)
		
		// Aborting is allowed from any state
		auto abortEvent = _trainingProgramFlow->abortRunningTrainingProgram();
		if (_currentTrainingProgram != nullptr)
		{
			_trainingProgramState = TrainingProgramState::WaitingForStart;
		}
		else
		{
			_trainingProgramState = TrainingProgramState::Uninitialized;
		}
		updateReadModel({ abortEvent });

	}
	void TrainingApplicationService::setGameState(GameState gameState)
	{
		_gameState = gameState;
		handlePauseChange();
	}
	void TrainingApplicationService::processTimerTick()
	{
		// Note: This method will be called _very_ often. Keep actions in here to a minimum
		if (_trainingProgramState != TrainingProgramState::Running)
		{
			return; // The "running" state is the only one in which we will ever need to make time comparisons
		}
		auto currentTrainingProgramEntryIndex = _trainingProgramFlow->currentTrainingStepNumber().value();
		// Note: We assume that the _trainingProgramEntryStartTimes are matching the training program while in the running state, for the sake of performance
		auto nextThreshold = _trainingProgramEntryEndTimes[currentTrainingProgramEntryIndex];

		auto passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _referenceTime);

		if (passedTime > nextThreshold)
		{
			std::shared_ptr<Kernel::DomainEvent> eventToBeForwarded;
			if (currentTrainingProgramEntryIndex == _trainingProgramEntryEndTimes.size() - 1)
			{
				eventToBeForwarded = _trainingProgramFlow->finishRunningTrainingProgram();
				_trainingProgramState = TrainingProgramState::WaitingForStart;
			}
			else
			{
				eventToBeForwarded = _trainingProgramFlow->activateNextTrainingProgramStep();
			}
			updateReadModel({ eventToBeForwarded });
		}
		_readModel.updateTrainingTime(passedTime);
	}

	void TrainingApplicationService::handlePauseChange()
	{
		auto isPaused = _trainingProgramFlow->runningTrainingProgramIsPaused();

		auto shouldBePaused = _gameState == GameState::Paused || _trainingProgramState == TrainingProgramState::Paused;

		bool stateHasBeenAdapted = false;
		if (!isPaused && shouldBePaused)
		{
			// The user has just triggered a pause by either pausing the program manually or by opening the ingame pause menu
			_pauseStartTime = std::chrono::steady_clock::now();
			stateHasBeenAdapted = true;
		}
		else if (isPaused && !shouldBePaused)
		{
			// The pause has ended. Move the reference Time forward in time by the duration of the pause.
			// That way we can simply calculate the difference to the reference time when we want to know how long the training program has been running
			_referenceTime = std::chrono::steady_clock::now() - (_pauseStartTime - _referenceTime);
			stateHasBeenAdapted = true;
		}

		if (stateHasBeenAdapted)
		{
			auto pauseOrResumeEvent = _trainingProgramFlow->pauseOrResumeTrainingProgram();
			updateReadModel({ pauseOrResumeEvent });
		}
	}
	void TrainingApplicationService::updateReadModel(std::vector<std::shared_ptr<Kernel::DomainEvent>> genericEvents)
	{
		for (const auto& genericEvent : genericEvents)
		{
			if (genericEvent == nullptr) { continue; }

			_readModel.dispatchEvent(genericEvent);
		}
	}

	TrainingProgramFlowReadModel TrainingApplicationService::getCurrentReadModel() const
	{
		return _readModel;
	}
}
