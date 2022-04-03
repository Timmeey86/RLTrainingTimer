//#pragma once
//
//#include <core/configuration/domain/TrainingProgramList.h>
//#include <core/training/events/TrainingProgramFlowEvents.h>
//
//#include <DLLImportExport.h>
//
//#include <vector>
//#include <optional>
//
//namespace Core::Training::Application
//{
//
//	/** Stores information about the training time. */
//	struct TrainingTimeInfo
//	{
//	public:
//		std::chrono::milliseconds TotalTrainingDuration = {};
//		std::chrono::milliseconds CurrentTrainingStepDuration = {};
//		std::chrono::milliseconds TimeLeftInProgram = {};
//		std::chrono::milliseconds TimeLeftInCurrentTrainingStep = {};
//		std::chrono::milliseconds TimeSpentInTraining = {};
//	};
//
//	/** Provides information about the current training step and information about the training time. */
//	class RLTT_IMPORT_EXPORT TrainingProgramDisplayReadModel
//	{
//	public:
//		TrainingProgramInfo SelectedTrainingProgramInfo;
//		TrainingStepInfo CurrentTrainingStepInfo;
//		TrainingTimeInfo TimeInfo;
//	};
//
//
//	/** This is a <<Read Model>> for the state of the Training Program Flow. 
//	 * It is meant to be used for determining what to display in the user interface.
//	 * 
//	 * Since we pass copies of this model around, we make the variables public - there's nothing we need to protect the members from.
//	 */
//	class TrainingProgramFlowReadModel
//	{
//	public:
//		/** Updates the read model with a new list of training program entries. */
//		void updateTrainingProgramListEntries(std::vector<Configuration::Domain::TrainingProgramListEntry> trainingProgramEntries);
//
//		/** Disables all actions since no training program is selected. */
//		void on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>& resetEvent);
//		/** Enables only the start action after a training program was selected. */
//		void on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent);
//		/** Enables only the Pause and Stop actions after a training program was selected and provides information about the first training step. */
//		void on(const std::shared_ptr<Events::TrainingProgramStartedEvent>& startEvent);
//		/** Enables only the Resume and Stop actions after a training program was paused. */
//		void on(const std::shared_ptr<Events::TrainingProgramPausedEvent>& pauseEvent);
//		/** Enables only the Pause and Stop actions after a training program was resumed. */
//		void on(const std::shared_ptr<Events::TrainingProgramResumedEvent>& resumeEvent);
//		/** Enables only the start action after a training program has finished. */
//		void on(const std::shared_ptr<Events::TrainingProgramFinishedEvent>& finishEvent);
//		/** Enables only the start action after a training program was aborted. */
//		void on(const std::shared_ptr<Events::TrainingProgramAbortedEvent>& abortEvent);
//		/** Updates information about the current training step. */
//		void on(const std::shared_ptr<Events::TrainingProgramStepActivatedEvent>& stepEvent);
//
//		/** Dispatches generic events into the discrete event handlers. */
//		void dispatchEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent);
//
//		/** Since this happens quite often, we don't create an event for it every time. */
//		inline void updateTrainingTime(
//			const std::chrono::milliseconds& trainingDuration,
//			const std::chrono::milliseconds& timeLeftInCurrentStep,
//			const std::chrono::milliseconds& timeLeftInProgram
//		) 
//		{
//			CurrentTrainingDuration = trainingDuration;
//			TimeLeftInCurrentStep = timeLeftInCurrentStep;
//			TimeLeftInProgram = timeLeftInProgram;
//		}
//
//	private:
//		void reset();
//
//	};
//}