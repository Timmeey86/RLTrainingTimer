#pragma once

#include <core/kernel/DomainEvent.h>

#include <vector>

namespace Core::Configuration::Events
{

	/** Signals a change in details of a training program. This allows figuring out whether or not the active training program has been changed.
	 */
	class TrainingProgramDetailsChangedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramDetailsChangedEvent() : Kernel::DomainEvent(true) {}

		std::vector<uint64_t> AffectedTrainingProgramIds;
	};

	/** Signals a name change of a training program. */
	class TrainingProgramRenamedEvent : public TrainingProgramDetailsChangedEvent
	{
	public:
		std::string TrainingProgramName;
	};

	/** Signals the addition of a new training program. */
	class TrainingProgramAddedEvent : public TrainingProgramDetailsChangedEvent
	{
	};

	/** Signals the deletion of a training program. */
	class TrainingProgramRemovedEvent : public TrainingProgramDetailsChangedEvent
	{
	};

	/** Signals the swap of two training programs. */
	class TrainingProgramSwappedEvent : public TrainingProgramDetailsChangedEvent
	{
	};

	/** Provides high-level information about one training program. */
	struct TrainingProgramInfo
	{
	public:
		uint64_t Id;
		uint16_t ListIndex;
		std::string Name;
		std::chrono::milliseconds Duration;
		uint16_t NumberOfSteps;
	};
	/** Signals a generic change in the training program list, i.e. anything which affects:
	 *
	 * - The amount of training programs
	 * - The order of training programs
	 * - The name of training programs
	 * - The duration of training programs
	 * - The number of training program steps
	 */
	class TrainingProgramListChangedEvent : public Kernel::DomainEvent 
	{
	public:
		TrainingProgramListChangedEvent() : Kernel::DomainEvent(false) {} // Event shall not be stored since it's just meant for easier UI updates

		std::vector<TrainingProgramInfo> TrainingProgramListInfo;
	};
}