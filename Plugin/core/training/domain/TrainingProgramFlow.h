#pragma once

#include "../definitions/TrainingProgramStates.h"

#include <core/kernel/DomainEvent.h>
#include <core/configuration/events/TrainingProgramEntryEvents.h>

#include <DLLImportExport.h>

#include <memory>
#include <optional>

namespace Core::Training::Domain
{

	/**
	 * This is an <<Aggregate Root>> which manages interactions related to state changes of the training program.
	 * 
	 * As of now, this object does not support restoring a previous state
	 *  
	 * Entities:
	 *  - None in addition to itself
	 * Value Objects:
	 *  - Selected Training Program Id (if any)
	 *  - "Running" state of the training program
	 *  - "Paused" state of the training program
	 *  - Current Training Step value (if set)
	 * Invariants:
	 *  - Only one training program can be selected at any given time
	 *  - Only the selected training program can have a state of "Running".
	 *  - Only a running training program can have a state of "Paused".
	 *  - Only a running training program can have a valid training step value.
	 *  - Only a running, unpaused training program can have the next state activated or be finished.
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramFlow
	{

	public:
		TrainingProgramFlow() = default;

		/** Handles a change in a training program. This usually doesn not generate events, unless a program which is currently running (or paused) has changed. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> handleTrainingProgramChange(const std::shared_ptr<Configuration::Events::TrainingProgramChangedEvent>& changeEvent);

		/** Marks a new training program as selected. This will stop any running training program. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> selectTrainingProgram(uint64_t trainingProgramId);

		/** Resets to the initial state where no training program is selected. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> unselectTrainingProgram();

		/** Starts the selected training program. Does nothing if no training program is selected.*/
		std::vector<std::shared_ptr<Kernel::DomainEvent>> startSelectedTrainingProgram();

		/** Activates the next (or first) step of the training program. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> activateNextTrainingProgramStep();

		/** Pauses the training program. Sends pause state events unless the game has been paused as well. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> pauseTrainingProgram();

		/** Resumes the training program (Note that the game might still be paused) */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> resumeTrainingProgram();

		/** Handles a game pause. Sends pause state events unless the training program has been paused as well. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> handleGamePauseStart();

		/** Resumes gameplay (Note that the training program might still be paused) */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> handleGamePauseEnd();

		/** Generates pause/resume events based on the current state. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> updatePauseState();

		/** Finishes the selected training program (usually only at the end of the training time). */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> finishRunningTrainingProgram();

		/** Aborts the training program by stopping it before it was finished. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> abortRunningTrainingProgram();

		/** Checks whether or not a program is currently selected. */
		inline bool trainingProgramIsSelected() const { return _selectedTrainingProgramId.has_value(); }

		/** Retrieves the current flow state. Mainly used for tests. */
		inline Definitions::TrainingProgramState currentTrainingProgramState() const { return _currentTrainingProgramState; }

		/** Retrieves the number of the current training step. */
		inline std::optional<uint16_t> currentTrainingStepNumber() const { return _currentTrainingStepNumber; }

		/** Retrieves the entries of the selected training program (if any). */
		std::vector<Configuration::Events::TrainingProgramEntryParams> getCurrentEntries();

		/** Creates a TrainingTimeUpdatedEvent for the given training duration and the currently active training step. */
		std::shared_ptr<Kernel::DomainEvent> createTimeUpdatedEvent(const std::chrono::milliseconds& passedTime, const std::chrono::milliseconds& nextThreshold);

		/** Checks whether the training program is currently in the running or a paused state (true), or a different state (false). */
		bool trainingProgramIsActive() const;

	private:

		std::vector<std::shared_ptr<Kernel::DomainEvent>> abortCurrentTrainingProgram(bool trainingWasFinished = false);
		void addStateEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>>& events, bool trainingWasFinished = false) const;

		std::optional<uint64_t> _selectedTrainingProgramId = {};
		std::optional<uint16_t> _currentTrainingStepNumber = {};
		Definitions::TrainingProgramState _currentTrainingProgramState = Definitions::TrainingProgramState::Uninitialized;

		Definitions::PausedState _trainingProgramPausedState = Definitions::PausedState::NotPaused;
		Definitions::PausedState _gamePausedState = Definitions::PausedState::NotPaused;

		std::unordered_map<uint64_t, Configuration::Events::TrainingProgramChangedEvent> _trainingProgramData;
	};
}