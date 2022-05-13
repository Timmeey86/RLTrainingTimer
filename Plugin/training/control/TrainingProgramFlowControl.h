#pragma once


#include <DLLImportExport.h>
#include <configuration/control/ITrainingProgramListReceiver.h>

#include "../data/TrainingProgramFlowData.h"
#include "../data/TrainingProgramExecutionData.h"

#include <bakkesmod/wrappers/gamewrapper.h>

#include <memory>
#include <optional>
#include <functional>

namespace training
{
	/** Defines the states a training program can be in. */
	enum class TrainingProgramState
	{
		Uninitialized, // No training program is selected
		WaitingForStart, // A training program has been selected and is waiting to be started
		Running, // A training program has been started after being selected. Neither the game nor the training program are paused.
		OnlyProgramPaused, // The training program has been paused, but the game has not
		OnlyGamePaused, // The training program is still "running", but the game is paused. The UI might still want to allow pausing the training program explicilty,
						// so it does not continue once the game gets unpaused.
		BothPaused, // Both the training program and the game are paused
	};

	/** Defines the state of something which can be paused (game and training program). */
	enum class PausedState
	{
		NotPaused, // The game is not paused
		Paused, // The game is paused
	};

	/**
	 * The job of this class is to keep the training program flow in a consistent state.
	 * 
	 * Invariants:
	 *  - Only one training program can be selected at any given time
	 *  - Only the selected training program can have a state of "Running".
	 *  - Only a running training program can have a state of "Paused".
	 *  - Only a running training program can have a valid training step value.
	 *  - Only a running, unpaused training program can have the next state activated or be finished.
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramFlowControl : public configuration::ITrainingProgramListReceiver
	{

	public:
		TrainingProgramFlowControl(std::shared_ptr<GameWrapper> gameWrapper);

		/** Hooks into Rocket League events in order to detect a game pause, and a game tick. */
		void hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper);

		/** Marks a new training program as selected. This will stop any running training program. */
		void selectTrainingProgram(uint64_t trainingProgramId);

		/** Resets to the initial state where no training program is selected. */
		void unselectTrainingProgram();

		/** Starts the selected training program. Does nothing if no training program is selected.*/
		void startSelectedTrainingProgram();

		/** Aborts the training program by stopping it before it was finished. */
		void stopRunningTrainingProgram();

		/** Pauses the training program. Sends pause state events unless the game has been paused as well. */
		void pauseTrainingProgram();

		/** Resumes the training program (Note that the game might still be paused) */
		void resumeTrainingProgram();

		/** Receives data from the configuration context. */
		void receiveListData(const configuration::TrainingProgramListData& data) override;

		/** Retrieves the current flow state (e.g. so an appropriate UI can be rendered). */
		inline TrainingProgramFlowData getCurrentFlowData() const { return _currentFlowData; }

		/** Retrieves the current execution state (e.g. so an appropriate UI can be rendered). */
		inline TrainingProgramExecutionData getCurrentExecutionData() const { return _currentExecutionData; }

	private:


		/** Handles a game pause. Sends pause state events unless the training program has been paused as well. */
		void handleGamePauseStart();
		/** Resumes gameplay (Note that the training program might still be paused) */
		void handleGamePauseEnd();
		/** Handles a timer tick. */
		void handleTimerTick();
		/** Checks whether the program is running or paused (true), or in a different state (false). */
		bool trainingProgramIsActive() const;
		/** Finishes the selected training program (usually only at the end of the training time). */
		void finishRunningTrainingProgram();
		/** Generates pause/resume events based on the current state. */
		void updatePauseState();
		/** Activates the next (or first) step of the training program. */
		void activateNextTrainingProgramStep();
		/** Switches to freeplay, custom training or whatever the user configured. */
		void switchGameModeIfNecessary(configuration::TrainingProgramEntry& trainingProgramEntry);
		/** Updates data for the UI based on the passed time and the threshold for the next step. */
		void updateTimeInfo(const std::chrono::milliseconds& passedTime, const std::chrono::milliseconds& nextThreshold);

		std::optional<uint64_t> _selectedTrainingProgramId = {};
		std::optional<uint16_t> _currentTrainingStepNumber = {};
		TrainingProgramState _currentTrainingProgramState = TrainingProgramState::Uninitialized;
		TrainingProgramFlowData _currentFlowData;
		TrainingProgramExecutionData _currentExecutionData;

		configuration::TrainingProgramListData _trainingProgramList;

		PausedState _trainingProgramPausedState = PausedState::NotPaused;
		PausedState _gamePausedState = PausedState::NotPaused;

		std::chrono::steady_clock::time_point _referenceTime; // The "start" time to do calculations again. Will be shifted to account for game pauses, if necessary.
		std::optional<std::chrono::steady_clock::time_point> _pauseStartTime = {}; // The point in time where a pause was started

		std::vector<std::chrono::milliseconds> _trainingProgramEntryEndTimes;

		std::shared_ptr<GameWrapper> _gameWrapper;
	};
}