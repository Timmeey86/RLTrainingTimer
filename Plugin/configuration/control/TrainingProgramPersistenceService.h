//#pragma once
//
//#include "TrainingProgramConfigurationService.h"
//
//#include <DLLImportExport.h>
//
//namespace Core::Configuration::Application
//{
//	/** An <<application service>> which allows storing and restoring of training programs. This must be triggered from outside of the domain context. 
//	 *
//	 * Since the configuration domain uses event sourcing, we implement storing and restoring by storing all configuration events which occurred.
//	*/
//	class RLTT_IMPORT_EXPORT TrainingProgramPersistenceService : public IConfigurationEventReceiver
//	{
//	public:
//		TrainingProgramPersistenceService(
//			const std::shared_ptr<GameWrapper>& gameWrapper,
//			const std::function<void(const std::vector<std::shared_ptr<Kernel::DomainEvent>>&)> replayEventsFunc
//		);
//
//		// Inherited via IConfigurationEventReceiver
//		void processEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent) override;
//		void postProcessEvents() override;
//
//		/** Stores all events in a persistent location. */
//		void storeAllEvents();
//		/** Restores and replays events from a persistent location. */
//		void restoreEvents();
//	private:
//		std::vector<std::shared_ptr<Kernel::DomainEvent>> _events;
//		std::filesystem::path _configurationStoragePath;
//		std::function<void(const std::vector<std::shared_ptr<Kernel::DomainEvent>>&)> _replayEventsFunc;
//		bool _eventsAreBeingRestored = false;
//	};
//}