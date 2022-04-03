#pragma once

#include <core/kernel/DomainEvent.h>
#include "../domain/TrainingProgramEntry.h"

#include <string>

#include <DLLImportExport.h>

namespace Core::Configuration::Events
{
	/** Signals a new entry which was added to a training program. */
	class TrainingProgramEntryAddedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramEntryAddedEvent() : Kernel::DomainEvent(true) {}

		uint64_t TrainingProgramId;
		std::string TrainingProgramEntryName;
		std::chrono::milliseconds TrainingProgramEntryDuration;
	};

	/** Signals the deletion of an entry of a training program. */
	class TrainingProgramEntryRemovedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramEntryRemovedEvent() : Kernel::DomainEvent(true) {}

		uint64_t TrainingProgramId;
		int TrainingProgramEntryPosition;
	};
	
	/** Signals a change in name of an entry of a training program. */
	class TrainingProgramEntryRenamedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramEntryRenamedEvent() : Kernel::DomainEvent(true) {}

		uint64_t TrainingProgramId;
		int TrainingProgramEntryPosition;
		std::string TrainingProgramEntryName;
	};
	/** Signals a change in duration of an entry of a training program. */
	class TrainingProgramEntryDurationChangedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramEntryDurationChangedEvent() : Kernel::DomainEvent(true) {}

		uint64_t TrainingProgramId;
		int TrainingProgramEntryPosition;
		std::chrono::milliseconds TrainingProgramEntryDuration;
	};

	/** Signals a swap of positions of entries of a training program. */
	class TrainingProgramEntrySwappedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramEntrySwappedEvent() : Kernel::DomainEvent(true) {}

		uint64_t TrainingProgramId;
		int FirstTrainingProgramEntryPosition;
		int SecondTrainingProgramEntryPosition;
	};

	/** Signals a generic change in the training program, i.e. anything which affects:
	 *
	 * - The amount of entries
	 * - The name of entries
	 * - The duration of entries (and thus also the duration of the program)
	 * - The name of the program
	 */
	class TrainingProgramChangedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramChangedEvent() : Kernel::DomainEvent(false) {} // Event shall not be stored since it's just meant for easier UI updates

		uint64_t TrainingProgramId;
		std::string TrainingProgramName;
		std::chrono::milliseconds TrainingProgramDuration;
		std::vector<Domain::TrainingProgramEntry> TrainingProgramEntries; // Note: We are forwarding a domain object here, but it's only a "dumb" value object.
	};
}