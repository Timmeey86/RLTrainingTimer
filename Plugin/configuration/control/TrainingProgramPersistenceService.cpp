#include <pch.h>

//#include "TrainingProgramPersistenceService.h"
//#include "event_serializing.hpp"
//
//#include <bakkesmod/wrappers/gamewrapper.h>
//
//#include <ostream>
//#include <fstream>
//namespace Core::Configuration::Application
//{
//
//
//	TrainingProgramPersistenceService::TrainingProgramPersistenceService(
//		const std::shared_ptr<GameWrapper>& gameWrapper,
//		const std::function<void(const std::vector<std::shared_ptr<Kernel::DomainEvent>>&)> replayEventsFunc)
//		: _replayEventsFunc{ replayEventsFunc }
//	{
//		auto configurationFolderPath = gameWrapper->GetDataFolder() / "RLTrainingTimer";
//		_configurationStoragePath = configurationFolderPath / "configuration.json";
//		if (!std::filesystem::exists(configurationFolderPath))
//		{
//			std::filesystem::create_directories(configurationFolderPath);
//		}
//
//		PolymorphicJsonSerializer<Kernel::DomainEvent>::register_types<
//			Kernel::DomainEvent,
//			Events::TrainingProgramDetailsChangedEvent,
//			Events::TrainingProgramRenamedEvent,
//			Events::TrainingProgramAddedEvent,
//			Events::TrainingProgramRemovedEvent,
//			Events::TrainingProgramSwappedEvent,
//			Events::TrainingProgramListChangedEvent,
//			Events::TrainingProgramEntryAddedEvent,
//			Events::TrainingProgramEntryRemovedEvent,
//			Events::TrainingProgramEntryRenamedEvent,
//			Events::TrainingProgramEntryDurationChangedEvent,
//			Events::TrainingProgramEntrySwappedEvent
//		>();
//	}
//	void TrainingProgramPersistenceService::processEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent)
//	{
//		if (genericEvent->shallBeStoredPersistently())
//		{
//			// Optimization: Changing a name sends an event for every keystroke. We squash these events down to only the last.
//			if (!_events.empty())
//			{
//				auto lastRenameEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramRenamedEvent>(_events.back());
//				auto currentRenameEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramRenamedEvent>(genericEvent);
//				if (lastRenameEvent != nullptr && currentRenameEvent != nullptr && lastRenameEvent->AffectedTrainingProgramIds.front() == currentRenameEvent->AffectedTrainingProgramIds.front())
//				{
//					_events.back() = genericEvent;
//					return;
//				}
//
//				auto lastEntryRenameEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramEntryRenamedEvent>(_events.back());
//				auto currentEntryRenameEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramEntryRenamedEvent>(genericEvent);
//				if (lastEntryRenameEvent != nullptr && currentEntryRenameEvent != nullptr &&
//					lastEntryRenameEvent->TrainingProgramId == currentEntryRenameEvent->TrainingProgramId &&
//					lastEntryRenameEvent->TrainingProgramEntryPosition == currentEntryRenameEvent->TrainingProgramEntryPosition)
//				{
//					_events.back() = genericEvent;
//					return;
//				}
//			}
//			// all other cases: simply store the event
//			_events.push_back(genericEvent);
//		}
//	}
//
//	void TrainingProgramPersistenceService::postProcessEvents()
//	{
//		if (!_eventsAreBeingRestored)
//		{
//			storeAllEvents();
//		}
//	}
//
//	void TrainingProgramPersistenceService::storeAllEvents()
//	{
//		json serialized = _events;
//		std::ofstream os{ _configurationStoragePath };
//		os << serialized.dump();
//		os.flush();
//	}
//
//	void TrainingProgramPersistenceService::restoreEvents()
//	{
//		if (!std::filesystem::exists(_configurationStoragePath)) { return; }
//
//		std::ifstream is{ _configurationStoragePath };
//		json deserialized;
//		is >> deserialized;
//		is.close();
//
//		_events.clear();
//
//		try
//		{
//			_eventsAreBeingRestored = true;
//			auto restoredEvents = deserialized.get<std::vector<std::shared_ptr<Kernel::DomainEvent>>>();
//			// Note: Executing this function will fill the _events object again, so we don't need to do it ourselves.
//			_replayEventsFunc(restoredEvents);
//		}
//		catch (std::runtime_error& ex)
//		{
//			LOG("Failed restoring training programs: {}", ex.what());
//		}
//		_eventsAreBeingRestored = false;
//		// We could store the events here, but that would produce the same file. We will write the file again when the next set of events gets sent our way.
//	}
//}
