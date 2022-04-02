#pragma once

#include <core/kernel/DomainEvent.h>
#include <stdint.h>
#include <optional>

namespace Core::Training::Events
{
	/** Signals the selection of a training program. Receiving this event means all other training programs are no longer selected. */
	class TrainingProgramSelectedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t SelectedTrainingProgramId;
		std::optional<uint64_t> PreviouslySelectedTrainingProgramId;
	};

	/** Signals a reset of the selection of a training program. */
	class TrainingProgramSelectionResetEvent : public Kernel::DomainEvent
	{
	public:
		std::optional<uint64_t> PreviouslySelectedTrainingProgramId;
	};

	/** Signals the start of a training program. */
	class TrainingProgramStartedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
	};

	/** Signals the end of a training program (Due to the time limit being reached). */
	class TrainingProgramFinishedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
	};

	/** Signals the beginning of a pause of a training event. */
	class TrainingProgramPausedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		bool GameIsPaused;
		bool TrainingProgramIsPaused;
	};

	/** Signals the end of a pause of a training event. */
	class TrainingProgramResumedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		bool GameIsPaused;
		bool TrainingProgramIsPaused;
	};

	/** Signals an unscheduled end of a training program, e.g. because the user stopped it in the middle, or closed the user interface. */
	class TrainingProgramAbortedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
	};

	/** Signals the start of a training program step. */
	class TrainingProgramStepActivatedEvent : public Kernel::DomainEvent
	{
	public:
		uint64_t TrainingProgramId;
		uint16_t TrainingProgramStepNumber;
		std::string TrainingProgramStepName;
		uint32_t TrainingProgramStepDuration;
	};
}