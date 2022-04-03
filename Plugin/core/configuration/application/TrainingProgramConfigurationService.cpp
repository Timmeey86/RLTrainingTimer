#include <pch.h>
#include "TrainingProgramConfigurationService.h"
#include "../events/TrainingProgramEvents.h"

namespace Core::Configuration::Application
{
	void TrainingProgramConfigurationService::registerEventReceiver(IConfigurationEventReceiver* const eventReceiver)
	{
		_eventReceivers.push_back(eventReceiver);
	}

	void TrainingProgramConfigurationService::addTrainingProgram(const Commands::AddTrainingProgramCommand&)
	{
		_maximumId++;
		auto events = _trainingProgramList->addTrainingProgram(_maximumId);

		// In addition to the regular events, we need to send an initial update so the UIs which focus on a single training program
		// know that this program exists now
		auto trainingProgram = _trainingProgramList->getTrainingProgram(_maximumId);
		publishEvents(trainingProgram->addProgramChangedEvent(events));
	}

	void TrainingProgramConfigurationService::removeTrainingProgram(const Commands::RemoveTrainingProgramCommand& command)
	{
		const auto events = _trainingProgramList->removeTrainingProgram(command.TrainingProgramId); // will throw if the ID does not exist
		publishEvents(events);
	}

	void TrainingProgramConfigurationService::renameTrainingProgram(const Commands::RenameTrainingProgramCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto renameEvent = trainingProgram->renameProgram(command.NewName);
		
		// Since the training program list contains the training program names, we need to add an additional event which propagates this change.
		publishEvents(_trainingProgramList->addListChangedEvent({ renameEvent }));
	}

	void TrainingProgramConfigurationService::swapTrainingPrograms(const Commands::SwapTrainingProgramCommand& command)
	{
		auto events = _trainingProgramList->swapTrainingPrograms(command.FirstTrainingProgramId, command.SecondTrainingProgramId);
		publishEvents(events);
	}

	void TrainingProgramConfigurationService::addTrainingProgramEntry(const Commands::AddTrainingProgramEntryCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto addEntryEvent = trainingProgram->addEntry({ "New training step", std::chrono::milliseconds(0) });

		// Since the training program list contains the number of training program entries, we need to add an additional event which propagates this change.
		publishEvents(_trainingProgramList->addListChangedEvent({ addEntryEvent }));
	}

	void TrainingProgramConfigurationService::removeTrainingProgramEntry(const Commands::RemoveTrainingProgramEntryCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto removeEntryEvent = trainingProgram->removeEntry(command.Position);

		// Since the training program list contains the number of training program entries, we need to add an additional event which propagates this change.
		publishEvents(_trainingProgramList->addListChangedEvent({ removeEntryEvent }));
	}

	void TrainingProgramConfigurationService::renameTrainingProgramEntry(const Commands::ChangeTrainingProgramEntryNameCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto renameEvent = trainingProgram->renameEntry(command.Position, command.NewName);

		// Since the entry name does not affect the list, we don't need to add an additional event here
		publishEvents({ renameEvent });
	}

	void TrainingProgramConfigurationService::changeTrainingProgramEntryDuration(const Commands::ChangeTrainingProgramEntryDurationCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto renameEvent = trainingProgram->changeEntryDuration(command.Position, command.Duration);

		// Since the training program list contains the training program duration, we need to add an additional event which propagates this change.
		publishEvents(_trainingProgramList->addListChangedEvent({ renameEvent }));
	}

	void TrainingProgramConfigurationService::swapTrainingProgramEntries(const Commands::SwapTrainingProgramEntriesCommand& command)
	{
		auto trainingProgram = _trainingProgramList->getTrainingProgram(command.TrainingProgramId);
		auto swapEvent = trainingProgram->swapEntries(command.FirstPosition, command.SecondPosition);

		// Since the training program list contains information about the list index for each training program,
		// we need to add an additional event which propagates this change.
		publishEvents(_trainingProgramList->addListChangedEvent({ swapEvent }));
	}

	void TrainingProgramConfigurationService::restoreTrainingProgramList(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& genericEvents)
	{
		_trainingProgramList->applyEvents(genericEvents);
		_maximumId = _trainingProgramList->getMaximumId();

		// We publish a single list change of the end result since this should be enough for any kind of UI
		publishEvents({ _trainingProgramList->createListChangedEvent() });
	}

	void TrainingProgramConfigurationService::publishEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events) const
	{
		for (const auto& eventReceiver : _eventReceivers)
		{
			for (auto genericEvent : events)
			{
				eventReceiver->processEvent(genericEvent);
			}
		}
	}
}
