#pragma once

#include <core/kernel/DomainEvent.h>
#include "../domain/TrainingProgramEntry.h"

#include <vector>

namespace Core::Configuration::Events
{
	/** Signals a name change of a training program. */
	class TrainingProgramRenamedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		std::string TrainingProgramName;
	};

	/** Signals the addition of a new training program. */
	class TrainingProgramAddedEvent : public Kernel::DomainEvent 
	{
	public:
		uint64_t TrainingProgramId;
	};

	/** Signals the deletion of a training program. */
	class TrainingProgramRemovedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
	};

	/** Signals the swap of two training programs. */
	class TrainingProgramSwappedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t FirstTrainingProgramId;
		uint64_t SecondTrainingProgramId;
	};
}