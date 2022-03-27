#pragma once

#include <core/kernel/DomainEvent.h>

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
	 * Invariants:
	 *  - Only one training program can be selected at any given time
	 *  - Only the selected training program can have a state of "Running".
	 *  - Only a running training program can have a state of "Paused".
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramInterface
	{

	public:
		TrainingProgramInterface() = default;

		/** Marks a new training program as selected. This will stop any running training program. */
		std::shared_ptr<Kernel::DomainEvent> selectTrainingProgram(uint64_t trainingProgramId);

		/** Resets to the initial state where no training program is selected. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> unselectTrainingProgram();

		/** Starts the selected training program. Does nothing if no training program is selected.*/
		std::shared_ptr<Kernel::DomainEvent> startSelectedTrainingProgram();

		/** Pauses or resumes the running training program. */
		std::shared_ptr<Kernel::DomainEvent> pauseOrResumeTrainingProgram();

		/** Finishes the selected training program (usually only at the end of the training time). */
		std::shared_ptr<Kernel::DomainEvent> finishRunningTrainingProgram();

		/** Aborts the training program by stopping it before it was finished. */
		std::shared_ptr<Kernel::DomainEvent> abortRunningTrainingProgram();

		/** Checks whether or not a program is currently selected. */
		inline bool trainingProgramIsSelected() const { return _selectedTrainingProgramId.has_value(); }

		/** Checks whether or not the selected training program is currently running. */
		inline bool selectedTrainingProgramIsRunning() const { return _selectedTrainingProgramIsRunning; }

		/** Checks whether or not the running training program is currently paused. */
		inline bool runningTrainingProgramIsPaused() const { return _runningTrainingProgramIsPaused; }

	private:
		std::optional<uint64_t> _selectedTrainingProgramId;
		bool _selectedTrainingProgramIsRunning = false;
		bool _runningTrainingProgramIsPaused = false;
	};
}