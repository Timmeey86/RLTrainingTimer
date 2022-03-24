#pragma once

#include <core/kernel/DomainEvent.h>

#include <string>

#include <DLLImportExport.h>

namespace Core::Configuration::Events
{
	/** Signals a new entry which was added to a training program. */
	class TrainingProgramEntryAddedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		std::string TrainingProgramEntryName;
		uint32_t TrainingProgramEntryDuration;
	};

	/** Signals the deletion of an entry of a training program. */
	class TrainingProgramEntryRemovedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		int TrainingProgramEntryPosition;
	};
	
	/** Signals an update of an entry of a training program. */
	class TrainingProgramEntryUpdatedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		int TrainingProgramEntryPosition;
		std::string TrainingProgramEntryName;
		uint32_t TrainingProgramEntryDuration;
	};

	/** Signals a swap of positions of entries of a training program. */
	class TrainingProgramEntrySwappedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		int FirstTrainingProgramEntryPosition;
		int SecondTrainingProgramEntryPosition;
	};
}