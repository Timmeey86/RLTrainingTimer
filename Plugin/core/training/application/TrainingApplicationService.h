//#pragma once
//
//#include "TrainingProgramDisplayReadModel.h"
//#include "TrainingProgramFlowControlReadModel.h"
//
//#include "../commands/TrainingCommands.h"
//
//#include "../domain/TrainingProgramFlow.h"
//
//#include <core/configuration/application/TrainingProgramConfigurationService.h> // For the event listener interface
//
//#include <DLLImportExport.h>
//
//#include <memory>
//
//namespace Core::Training::Application
//{
//	/** This is an <<application service>> which processes input from the outside world and translates it into domain commands. 
//	 * In the first version, this will be a <<domain service>> and an <<application service>> combined.
//	 *
//	 * This class will keep track of the time played.
//	 * In a larger project, this would be an actual service. Right here, it's just a class.
//	 * 
//	 * In a larger project, this would also be asynchronous and stateles, but this plugin only supports a single user and a single program at all.
//	 */
//	class RLTT_IMPORT_EXPORT TrainingApplicationService : public Configuration::Application::IConfigurationEventReceiver
//	{
//	public:
//		/** Constructor. */
//		explicit TrainingApplicationService();
//
//		/** Selects a program for the upcoming training. */
//		void selectTrainingProgram(const Commands::SelectTrainingProgramCommand& command);
//
//		/** Starts the most recently selected training program. */
//		void startTrainingProgram(const Commands::StartTrainingProgramCommand& command);
//
//		/** Pauses a currently running training program. */
//		void pauseTrainingProgram(const Commands::PauseTrainingProgramCommand& command);
//
//		/** Resumes a currently paused training program. */
//		void resumeTrainingProgram(const Commands::ResumeTrainingProgramCommand& command);
//
//		/** Aborts a training program ahead of time. */
//		void abortTrainingProgram(const Commands::AbortTrainingProgramCommand& command);
//
//		/** Reacts to a change in the state of the game itself. */
//		void setGameState(Definitions::PausedState gameState);
//
//		/** Processes a timer tick synchronously. */
//		void processTimerTick();
//
//		/** Updates the internal information based on the events feeded from the configuration domain. */
//		void processEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent) override;
//		void postProcessEvents() override {}
//
//		/** Retrieves the read model for the training program flow control. */
//		inline TrainingProgramFlowControlReadModel getFlowControlReadModel() const { return _flowControlReadModel; }
//		
//		/** Retrieves the read model for the training program display. */
//		inline TrainingProgramDisplayReadModel getDisplayReadModel() const { return _displayReadModel; }
//
//	private:
//
//		/** Updates the read model with a list of events  */
//		void updateReadModels(std::vector<std::shared_ptr<Kernel::DomainEvent>> genericEvents);
//		void handlePauseChange();
//
//		std::chrono::steady_clock::time_point _referenceTime; // The "start" time to do calculations again. Will be shifted to account for game pauses, if necessary.
//		std::optional<std::chrono::steady_clock::time_point> _pauseStartTime = {}; // The point in time where a pause was started
//		
//		std::vector<std::chrono::milliseconds> _trainingProgramEntryEndTimes;
//
//		std::unique_ptr<Domain::TrainingProgramFlow> _trainingProgramFlow = std::make_unique<Domain::TrainingProgramFlow>();
//		TrainingProgramDisplayReadModel _displayReadModel;
//		TrainingProgramFlowControlReadModel _flowControlReadModel;
//
//	};
//}