//#pragma once
//
//#include <core/kernel/DomainEvent.h>
//
//#include <DLLImportExport.h>
//
//#include <memory>
//#include <optional>
//
//namespace Core::Training::Domain
//{
//
//	/**
//	 * This is an <<Aggregate Root>> which manages interactions related to state changes of the training program.
//	 * 
//	 * As of now, this object does not support restoring a previous state
//	 * 
//	 * Entities:
//	 *  - None in addition to itself
//	 * Value Objects:
//	 *  - Selected Training Program Id (if any)
//	 *  - "Running" state of the training program
//	 *  - "Paused" state of the training program
//	 *  - Current Training Step value (if set)
//	 * Invariants:
//	 *  - Only one training program can be selected at any given time
//	 *  - Only the selected training program can have a state of "Running".
//	 *  - Only a running training program can have a state of "Paused".
//	 *  - Only a running training program can have a valid training step value.
//	 *  - Only a running, unpaused training program can have the next state activated or be finished.
//	 */
//	class RLTT_IMPORT_EXPORT TrainingProgramFlow
//	{
//
//	public:
//		TrainingProgramFlow() = default;
//
//		/** Marks a new training program as selected. This will stop any running training program. */
//		std::vector<std::shared_ptr<Kernel::DomainEvent>> selectTrainingProgram(uint64_t trainingProgramId, uint16_t numberOfTrainingProgramSteps);
//
//		/** Resets to the initial state where no training program is selected. */
//		std::vector<std::shared_ptr<Kernel::DomainEvent>> unselectTrainingProgram();
//
//		/** Starts the selected training program. Does nothing if no training program is selected.*/
//		std::shared_ptr<Kernel::DomainEvent> startSelectedTrainingProgram();
//
//		/** Activates the next (or first) step of the training program. */
//		std::shared_ptr<Kernel::DomainEvent> activateNextTrainingProgramStep();
//
//		/** Pauses or resumes the running training program. */
//		std::shared_ptr<Kernel::DomainEvent> pauseOrResumeTrainingProgram(bool gameIsPaused, bool trainingProgramIsPaused);
//
//		/** Finishes the selected training program (usually only at the end of the training time). */
//		std::shared_ptr<Kernel::DomainEvent> finishRunningTrainingProgram();
//
//		/** Aborts the training program by stopping it before it was finished. */
//		std::shared_ptr<Kernel::DomainEvent> abortRunningTrainingProgram();
//
//		/** Checks whether or not a program is currently selected. */
//		inline bool trainingProgramIsSelected() const { return _selectedTrainingProgramId.has_value(); }
//
//		/** Checks whether or not the selected training program is currently running. */
//		inline bool selectedTrainingProgramIsRunning() const { return _selectedTrainingProgramIsRunning; }
//
//		/** Checks whether or not the running training program is currently paused. */
//		inline bool runningTrainingProgramIsPaused() const { return _runningTrainingProgramIsPaused; }
//
//		/** Retrieves the number of the current training step. */
//		inline std::optional<uint16_t> currentTrainingStepNumber() const { return _currentTrainingStepNumber; }
//
//	private:
//
//		std::shared_ptr<Core::Kernel::DomainEvent> abortCurrentTrainingProgram();
//
//		std::optional<uint64_t> _selectedTrainingProgramId;
//		bool _selectedTrainingProgramIsRunning = false;
//		bool _runningTrainingProgramIsPaused = false;
//		std::optional<uint16_t> _currentTrainingStepNumber;
//		uint16_t _maxTrainingStepNumber = 0;
//	};
//}