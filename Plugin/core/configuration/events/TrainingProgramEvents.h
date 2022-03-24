#pragma once

#include <core/kernel/DomainEvent.h>
#include "../domain/TrainingProgramEntry.h"

#include <vector>

namespace Core::Configuration::Events
{
	/** Signals a name change of a training program.
	 */
	class TrainingProgramRenamedEvent : Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		std::string TrainingProgramName;
	};
}