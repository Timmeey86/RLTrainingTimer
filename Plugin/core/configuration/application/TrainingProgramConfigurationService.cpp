#include <pch.h>
#include "TrainingProgramConfigurationService.h"
#include "../events/TrainingProgramEvents.h"
#include "../events/TrainingProgramEntryEvents.h"

namespace Core::Configuration::Application
{
	void TrainingProgramConfigurationService::registerEventReceiver(IConfigurationEventReceiver* const eventReceiver)
	{
		_eventReceivers.push_back(eventReceiver);
	}

	void TrainingProgramConfigurationService::applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
	{
		// Note: The original plan was to let objects restore themselves through events. Currently, this won't work since replaying the stored events would send different events
		//       than were sent originally, since we are preventing some events from being saved. We might have to reconsider this if we want to get rid of this ugly method.
		std::unordered_map<uint64_t, uint64_t> oldToNewIdHash;
		for (auto genericEvent : events)
		{
			if (auto additionEvent = dynamic_cast<Events::TrainingProgramAddedEvent*>(genericEvent.get()))
			{
				addTrainingProgram({});
				auto oldId = additionEvent->AffectedTrainingProgramIds.front();
				auto newId = _trainingProgramList->getMaximumId();
				oldToNewIdHash[oldId] = newId;
			}
			else if (auto removalEvent = dynamic_cast<Events::TrainingProgramRemovedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[removalEvent->AffectedTrainingProgramIds.front()];
				removeTrainingProgram({ newId });
			}
			else if (auto swapEvent = dynamic_cast<Events::TrainingProgramSwappedEvent*>(genericEvent.get()))
			{
				auto newFirstId = oldToNewIdHash[swapEvent->AffectedTrainingProgramIds.front()];
				auto newSecondId = oldToNewIdHash[swapEvent->AffectedTrainingProgramIds.back()];
				swapTrainingPrograms({ newFirstId, newSecondId });
			}
			else if (auto renameEvent = dynamic_cast<Events::TrainingProgramRenamedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[renameEvent->AffectedTrainingProgramIds.front()];
				renameTrainingProgram({ newId, renameEvent->TrainingProgramName });
			}
			else if (auto entryAdditionEvent = dynamic_cast<Events::TrainingProgramEntryAddedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[entryAdditionEvent->TrainingProgramId];

				addTrainingProgramEntry({ newId });
			}
			else if (auto entryRemovalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[entryRemovalEvent->TrainingProgramId];

				removeTrainingProgramEntry({ newId });
			}
			else if (auto entryRenameEvent = dynamic_cast<Events::TrainingProgramEntryRenamedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[entryRenameEvent->TrainingProgramId];

				renameTrainingProgramEntry({ newId, (uint16_t)entryRenameEvent->TrainingProgramEntryPosition, entryRenameEvent->TrainingProgramEntryName });
			}
			else if (auto entryDurationChangeEvent = dynamic_cast<Events::TrainingProgramEntryDurationChangedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[entryDurationChangeEvent->TrainingProgramId];

				changeTrainingProgramEntryDuration({ newId, (uint16_t)entryDurationChangeEvent->TrainingProgramEntryPosition, entryDurationChangeEvent->TrainingProgramEntryDuration });
			}
			else if (auto entrySwapEvent = dynamic_cast<Events::TrainingProgramEntrySwappedEvent*>(genericEvent.get()))
			{
				auto newId = oldToNewIdHash[entrySwapEvent->TrainingProgramId];

				swapTrainingProgramEntries({ newId, (uint16_t)entrySwapEvent->FirstTrainingProgramEntryPosition, (uint16_t)entrySwapEvent->SecondTrainingProgramEntryPosition });
			}
		}
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

	void TrainingProgramConfigurationService::publishEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events) const
	{
		for (const auto& eventReceiver : _eventReceivers)
		{
			for (auto genericEvent : events)
			{
				eventReceiver->processEvent(genericEvent);
			}
			eventReceiver->postProcessEvents();
		}
	}
}
