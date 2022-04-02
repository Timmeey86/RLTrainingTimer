#pragma once

#include <DLLImportExport.h>
#include "TrainingProgramFlowReadModel.h"
#include "../commands/TrainingCommands.h"
#include "../domain/TrainingProgramFlow.h"
#include <core/configuration/domain/TrainingProgram.h>
#include <core/configuration/domain/TrainingProgramList.h>

#include <memory>

namespace Core::Training::Application
{
	/** Defines the state the training program can be in. */
	enum class TrainingProgramState
	{
		Uninitialized, // No training program is selected
		WaitingForStart, // A training program has been selected and is waiting to be started
		Running, // A training program has been started after being selected
		Paused, // A training program has been paused after it was started. Resuming will go back to "Running"
	};

	/** Defines the state of the game. */
	enum class GameState
	{
		NotPaused, // The game is not paused
		Paused, // The game is paused
	};
	/** This is an <<application service>> which processes input from the outside world and translates it into domain commands. 
	 * In the first version, this will be a <<domain service>> and an <<application service>> combined.
	 *
	 * This class will: Keep track of the time played.
	 * In a larger project, this would be an actual service. Right here, it's just a class.
	 * 
	 * In a larger project, this would also be asynchronous and stateles, but this plugin only supports a single user and a single program at all.
	 */
	class RLTT_IMPORT_EXPORT TrainingApplicationService
	{
	public:
		/** Constructor. */
		explicit TrainingApplicationService(
			std::shared_ptr<Configuration::Domain::TrainingProgramList> trainingProgramList,
			std::shared_ptr<Domain::TrainingProgramFlow> trainingProgramFlow);

		/** Selects a program for the upcoming training. */
		void selectTrainingProgram(const Commands::SelectTrainingProgramCommand& command);

		/** Starts the most recently selected training program. */
		void startTrainingProgram(const Commands::StartTrainingProgramCommand& command);

		void extendStepActivationEvent(std::shared_ptr<Core::Kernel::DomainEvent>& firstStepActivationEvent);

		/** Pauses a currently running training program. */
		void pauseTrainingProgram(const Commands::PauseTrainingProgramCommand& command);

		/** Resumes a currently paused training program. */
		void resumeTrainingProgram(const Commands::ResumeTrainingProgramCommand& command);

		/** Aborts a training program ahead of time. */
		void abortTrainingProgram(const Commands::AbortTrainingProgramCommand& command);

		/** Reacts to a change in the state of the game itself. */
		void setGameState(GameState gameState);

		/** Processes a timer tick synchronously. */
		void processTimerTick();

		/** Retrieves a copy of the current read model for the training program flow. This could be an own service class, but it would only return this object and do nothing else. */
		TrainingProgramFlowReadModel getCurrentReadModel() const;

	private:

		/** Updates the "paused" state dependent on if the user has manually paused the program and if the ingame pause menu is open. */
		void handlePauseChange();

		/** Updates the read model with a list of events  */
		void updateReadModel(std::vector<std::shared_ptr<Kernel::DomainEvent>> genericEvents);

		TrainingProgramState _trainingProgramState = TrainingProgramState::Uninitialized;
		GameState _gameState = GameState::NotPaused;
		std::chrono::steady_clock::time_point _referenceTime; // The "start" time to do calculations again. Will be shifted to account for game pauses, if necessary.
		std::chrono::steady_clock::time_point _pauseStartTime; // The point in time where a pause was started
		
		std::shared_ptr<Configuration::Domain::TrainingProgramList> _trainingProgramList = nullptr; // Provides access to the known training programs.
		std::shared_ptr<Domain::TrainingProgramFlow> _trainingProgramFlow = nullptr;
		TrainingProgramFlowReadModel _readModel;

		std::shared_ptr<Configuration::Domain::TrainingProgram> _currentTrainingProgram = nullptr;
		std::vector<std::chrono::milliseconds> _trainingProgramEntryEndTimes;
	};
}