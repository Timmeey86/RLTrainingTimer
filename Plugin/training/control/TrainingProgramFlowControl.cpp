#include <pch.h>
#include "TrainingProgramFlowControl.h"

#include <unordered_set>

namespace training
{
	TrainingProgramFlowControl::TrainingProgramFlowControl(
		std::shared_ptr<IGameWrapper> gameWrapper, 
		std::shared_ptr<ITimeProvider> timeProvider,
		std::shared_ptr<ICVarManager> cvarManager)
		: _gameWrapper{ std::move(gameWrapper) }
		, _timeProvider{ std::move(timeProvider) }
		, _cvarManager{ std::move(cvarManager) }
	{

	}

	void TrainingProgramFlowControl::hookToEvents()
	{
		_gameWrapper->HookEventPost("Function Engine.WorldInfo.EventPauseChanged", [this](const std::string&) {
			if (_gameWrapper->IsPaused())
			{
				handleGamePauseStart();
			}
			else
			{
				handleGamePauseEnd();
			}
		});

		_gameWrapper->HookEvent("Function TAGame.Replay_TA.Tick", [this](const std::string&) {
			handleTimerTick();
		});

		_currentFlowData.SwitchingIsPossible = true; // We currently always allow switching
	}

	void TrainingProgramFlowControl::selectTrainingProgram(const std::string& trainingProgramId)
	{
		if (_trainingProgramList.TrainingProgramData.count(trainingProgramId) > 0)
		{
			// Adapt internal state                    
			_selectedTrainingProgramId = trainingProgramId;
			_currentTrainingStepNumber.reset();

			// Provide information for the flow control UI
			_currentTrainingProgramState = TrainingProgramState::WaitingForStart;
			_currentFlowData.SelectedTrainingProgramIndex = (uint16_t)std::distance(
				_trainingProgramList.TrainingProgramOrder.begin(),
				std::find(_trainingProgramList.TrainingProgramOrder.begin(), _trainingProgramList.TrainingProgramOrder.end(), trainingProgramId));
			_currentFlowData.StartingIsPossible = true;
			_currentFlowData.PausingIsPossible = false;
			_currentFlowData.ResumingIsPossible = false;
			_currentFlowData.StoppingIsPossible = false;

			// Don't provide information for the training UI just yet (but do so once the program gets started
			_currentExecutionData.NumberOfSteps = 0;
			_currentExecutionData.TrainingIsPaused = false;
		}
		else
		{
			unselectTrainingProgram(); // Returns to the initial state where nothing is selected
		}
	}

	void TrainingProgramFlowControl::unselectTrainingProgram()
	{
		_selectedTrainingProgramId.reset();
		_currentTrainingStepNumber.reset();

		// Provide information for the flow control UI
		_currentTrainingProgramState = TrainingProgramState::Uninitialized;
		_currentFlowData.SelectedTrainingProgramIndex.reset();
		_currentFlowData.StartingIsPossible = false;
		_currentFlowData.PausingIsPossible = false;
		_currentFlowData.ResumingIsPossible = false;
		_currentFlowData.StoppingIsPossible = false;

		_currentExecutionData.NumberOfSteps = 0; // Invalidates everything else
	}

	void TrainingProgramFlowControl::startSelectedTrainingProgram()
	{
		if (_selectedTrainingProgramId.has_value() && !trainingProgramIsActive())
		{
			// Precalculate program steps
			auto trainingProgramEntryEndTime = std::chrono::milliseconds(0);
			_trainingProgramEntryEndTimes.clear();
			const auto& entries = _trainingProgramList.TrainingProgramData[_selectedTrainingProgramId.value()].Entries;
			for (const auto& trainingProgramEntry : entries)
			{
				trainingProgramEntryEndTime += std::chrono::milliseconds(trainingProgramEntry.Duration);
				_trainingProgramEntryEndTimes.push_back(trainingProgramEntryEndTime);
			}

			// Provide information for the flow control UI
			_currentTrainingProgramState = TrainingProgramState::Running;
			_currentFlowData.StartingIsPossible = false;
			_currentFlowData.PausingIsPossible = true;
			_currentFlowData.ResumingIsPossible = false;
			_currentFlowData.StoppingIsPossible = true;

			_currentExecutionData.TrainingFinishedTime.reset();
			_currentTrainingStepNumber.reset();

			// Automatically activate the first step
			_referenceTime = _timeProvider->now();
			activateNextTrainingProgramStep();
		}
	}

	void TrainingProgramFlowControl::activateNextTrainingProgramStep()
	{
		if (_selectedTrainingProgramId.has_value()
			&& _currentTrainingProgramState == TrainingProgramState::Running
			&& _trainingProgramList.TrainingProgramData.count(_selectedTrainingProgramId.value()) > 0)
		{
			const auto& trainingProgramData = _trainingProgramList.TrainingProgramData.at(_selectedTrainingProgramId.value());

			if (!trainingProgramData.Entries.empty() &&
				(!_currentTrainingStepNumber.has_value() || _currentTrainingStepNumber < trainingProgramData.Entries.size() - 1))
			{
				if (!_currentTrainingStepNumber.has_value())
				{
					_currentTrainingStepNumber = 0;
				}
				else
				{
					_currentTrainingStepNumber = _currentTrainingStepNumber.value() + 1;
				}
				auto trainingProgramEntry = trainingProgramData.Entries.at(_currentTrainingStepNumber.value());

				switchGameModeIfNecessary(trainingProgramEntry);
				// No change in flow state (still running)

				// Provide information for the training execution UI
				_currentExecutionData.NumberOfSteps = (uint16_t)trainingProgramData.Entries.size();
				_currentExecutionData.TrainingProgramName = trainingProgramData.Name;
				_currentExecutionData.TrainingStepNumber = _currentTrainingStepNumber.value();
				_currentExecutionData.TrainingStepName = trainingProgramEntry.Name;
				_currentExecutionData.DurationOfCurrentTrainingStep = trainingProgramEntry.Duration;
				_currentExecutionData.TimeLeftInCurrentTrainingStep = trainingProgramEntry.Duration; // will be reduced in handleTimerTick()
				_currentExecutionData.TimeLeftInProgram = trainingProgramData.Duration;
				_currentExecutionData.TrainingFinishedTime.reset();
				_currentExecutionData.TrainingStepStartTime = _timeProvider->now();
			}
			else if(trainingProgramData.Entries.empty())
			{
				// The training program is empty, there is nothing we can do with it. We simulate unselection & selection to return to the "selected" state
				unselectTrainingProgram();
				selectTrainingProgram(trainingProgramData.Id);
			}
		}
	}

	void TrainingProgramFlowControl::switchGameModeIfNecessary(configuration::TrainingProgramEntry& trainingProgramEntry)
	{
		switch (trainingProgramEntry.Type)
		{
		case configuration::TrainingProgramEntryType::Freeplay:
			if (!_gameWrapper->IsInFreeplay())
			{
				_gameWrapper->Execute([this](GameWrapper*) { _cvarManager->executeCommand("load_freeplay"); });
			}
			break;
		case configuration::TrainingProgramEntryType::CustomTraining:
			_gameWrapper->Execute([this, trainingProgramEntry](GameWrapper*) { _cvarManager->executeCommand(fmt::format("load_training {}", trainingProgramEntry.TrainingPackCode)); });
			break;
		case configuration::TrainingProgramEntryType::WorkshopMap:
			_gameWrapper->Execute([this, trainingProgramEntry](GameWrapper*) { _cvarManager->executeCommand(fmt::format("load_workshop \"{}\\{}\"", _trainingProgramList.WorkshopFolderLocation, trainingProgramEntry.WorkshopMapPath)); });
		default:
			// Do nothing
			break;
		}
	}

	void TrainingProgramFlowControl::pauseTrainingProgram()
	{
		_trainingProgramPausedState = PausedState::Paused;
		updatePauseState();
	}
	void TrainingProgramFlowControl::resumeTrainingProgram()
	{
		_trainingProgramPausedState = PausedState::NotPaused;
		updatePauseState();
	}

	void TrainingProgramFlowControl::handleGamePauseStart()
	{
		_gamePausedState = PausedState::Paused;
		updatePauseState();
	}

	void TrainingProgramFlowControl::handleGamePauseEnd()
	{
		_gamePausedState = PausedState::NotPaused;
		updatePauseState();
	}

	void TrainingProgramFlowControl::handleTimerTick()
	{
		if (_currentTrainingProgramState != TrainingProgramState::Running || !_currentTrainingStepNumber.has_value())
		{
			return; // The "running" state is the only one in which we will ever need to make time comparisons
		}
		auto currentTrainingStepNumber = _currentTrainingStepNumber.value();

		auto nextThreshold = _trainingProgramEntryEndTimes[currentTrainingStepNumber];
		auto passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(_timeProvider->now() - _referenceTime);

		if (passedTime > nextThreshold)
		{
			if (currentTrainingStepNumber == _trainingProgramEntryEndTimes.size() - 1)
			{
				finishRunningTrainingProgram();
			}
			else
			{
				// The end of the current step has been reached, and there is another one
				activateNextTrainingProgramStep();
				// Note: We don't update time info here, this method will be called again within a couple of milliseconds, which is good enough
			}
		}
		else
		{
			updateTimeInfo(passedTime, nextThreshold);
		}
	}

	void TrainingProgramFlowControl::updatePauseState()
	{
		// Find out what is paused and if a state has changed
		const auto trainingProgramIsPaused = _trainingProgramPausedState == PausedState::Paused;
		const auto gameIsPaused = _gamePausedState == PausedState::Paused;

		// Transition through the internal states only when in the Running state or one of the paused states
		// Pausing the game in a different state will send a state update for the UI, but will not transition to a different state
		// Pausing the training program should only be possible in the "Running" and "OnlyGamePaused" states
		if (trainingProgramIsActive())
		{
			auto flowWasPaused = (_currentTrainingProgramState != TrainingProgramState::Running);

			if (gameIsPaused && !trainingProgramIsPaused)
			{
				_currentTrainingProgramState = TrainingProgramState::OnlyGamePaused;
			}
			else if (!gameIsPaused && trainingProgramIsPaused)
			{
				_currentTrainingProgramState = TrainingProgramState::OnlyProgramPaused;
			}
			else if (gameIsPaused && trainingProgramIsPaused)
			{
				_currentTrainingProgramState = TrainingProgramState::BothPaused;
			}
			else
			{
				_currentTrainingProgramState = TrainingProgramState::Running;
			}

			auto flowIsPaused = (_currentTrainingProgramState != TrainingProgramState::Running);

			if (!flowWasPaused && flowIsPaused)
			{
				// Pause has started
				if (!_pauseStartTime.has_value())
				{
					// Rememeber the time when the pause started
					_pauseStartTime = _timeProvider->now();
				}
			}
			else if (flowWasPaused && !flowIsPaused)
			{
				// Pause has ended
				if (_pauseStartTime.has_value())
				{
					// Shift the reference time forward by the duration of the pause. This way, calculations can use this time and act as if there hasn't been any pause.
					_referenceTime = _timeProvider->now() - (_pauseStartTime.value() - _referenceTime);
					_pauseStartTime.reset();
				}
			}
			// else: game was paused and is still paused (the fourth case should be impossible at this point)

			// Update the UI buttons (Note that the game state does not matter for this)
			_currentFlowData.PausingIsPossible = !trainingProgramIsPaused;
			_currentFlowData.ResumingIsPossible = trainingProgramIsPaused;
		}

		// Allow displaying the "Paused" state in the execution UI
		_currentExecutionData.TrainingIsPaused = gameIsPaused || trainingProgramIsPaused;
	}

	void TrainingProgramFlowControl::finishRunningTrainingProgram()
	{
		if (_selectedTrainingProgramId.has_value() && _currentTrainingProgramState == TrainingProgramState::Running)
		{
			stopRunningTrainingProgram();
			_currentExecutionData.TrainingFinishedTime = _timeProvider->now();
		}
		// Else: Rather than throwing an exception, ignore this.
	}

	void TrainingProgramFlowControl::stopRunningTrainingProgram()
	{
		if (_selectedTrainingProgramId.has_value())
		{
			// Provide information for the flow control UI
			_currentTrainingProgramState = TrainingProgramState::WaitingForStart;
			_currentFlowData.StartingIsPossible = true;
			_currentFlowData.PausingIsPossible = false;
			_currentFlowData.ResumingIsPossible = false;
			_currentFlowData.StoppingIsPossible = false;

			_currentExecutionData.TrainingStepStartTime.reset();
		}
	}

	void TrainingProgramFlowControl::updateTimeInfo(const std::chrono::milliseconds& passedTime, const std::chrono::milliseconds& nextThreshold)
	{
		if (_selectedTrainingProgramId.has_value() && _trainingProgramList.TrainingProgramData.count(_selectedTrainingProgramId.value()) > 0)
		{
			const auto& currentTrainingProgram = _trainingProgramList.TrainingProgramData.at(_selectedTrainingProgramId.value());
			if (_currentTrainingStepNumber.has_value() && _currentTrainingStepNumber.value() < currentTrainingProgram.Entries.size())
			{
				_currentExecutionData.TimeLeftInCurrentTrainingStep = nextThreshold - passedTime;
				_currentExecutionData.TimeLeftInProgram = currentTrainingProgram.Duration - passedTime;

				// Fixup negative values (happens when "passedTime" doesn't match the training program duration exactly, which will probably happen almost always)
				if (_currentExecutionData.TimeLeftInProgram.count() < 0)
				{
					_currentExecutionData.TimeLeftInProgram = std::chrono::milliseconds(0);
				}
				if (_currentExecutionData.TimeLeftInCurrentTrainingStep.count() < 0)
				{
					_currentExecutionData.TimeLeftInCurrentTrainingStep = std::chrono::milliseconds(0);
				}
			}
		}
	}

	bool TrainingProgramFlowControl::trainingProgramIsActive() const
	{
		// The program is "active" if it is either running or paused
		auto relevantStates = std::unordered_set<TrainingProgramState>{
			TrainingProgramState::Running,
			TrainingProgramState::OnlyGamePaused,
			TrainingProgramState::OnlyProgramPaused,
			TrainingProgramState::BothPaused
		};
		return relevantStates.count(_currentTrainingProgramState) > 0;
	}

	void TrainingProgramFlowControl::receiveListData(const configuration::TrainingProgramListData& data)
	{
		stopRunningTrainingProgram();
		_trainingProgramList = data;
		_currentFlowData.TrainingPrograms.clear();
		for (auto trainingProgramId : data.TrainingProgramOrder)
		{
			_currentFlowData.TrainingPrograms.push_back({ trainingProgramId, data.TrainingProgramData.at(trainingProgramId).Name });
		}
	}
}
