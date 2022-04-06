#pragma once

#include <core/kernel/DomainEvent.h>
#include <stdint.h>
#include <optional>
#include <chrono>

namespace Core::Training::Events
{
	/** Signals the selection of a training program. Receiving this event means all other training programs are no longer selected. */
	class TrainingProgramSelectedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramSelectedEvent() : Kernel::DomainEvent(false) {} // We only store&restore configuration events

		uint64_t SelectedTrainingProgramId = 0;
		std::optional<uint64_t> PreviouslySelectedTrainingProgramId = {};
		std::string Name = {};
		std::chrono::milliseconds Duration = {};
		uint16_t NumberOfSteps = 0;
	};

	/** Signals a reset of the selection of a training program. */
	class TrainingProgramSelectionResetEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramSelectionResetEvent() : Kernel::DomainEvent(false) {} // We only store&restore configuration events

		std::optional<uint64_t> PreviouslySelectedTrainingProgramId;
	};

	/** Signals a state change in the training program flow. */
	class TrainingProgramStateChangedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramStateChangedEvent() : Kernel::DomainEvent(false) {} // We only store&restore configuration events

		bool StartingIsPossible = false;
		bool PausingIsPossible = false;
		bool ResumingIsPossible = false;
		bool StoppingIsPossible = false;
		bool SwitchingProgramIsPossible = false;
	};

	/** Signals the change to a new training program step. The valid flag is used for invalidating any currently displayed training step info. */
	class TrainingProgramStepChangedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingProgramStepChangedEvent() : Kernel::DomainEvent(false) {} // We only store&restore configuration events

		bool IsValid = false;
		std::string Name = {};
		std::chrono::milliseconds Duration = {};
		uint16_t StepNumber = 0;
	};

	/** Signals a change in training time. */
	class TrainingTimeUpdatedEvent : public Kernel::DomainEvent
	{
	public:
		TrainingTimeUpdatedEvent() : Kernel::DomainEvent(false) {} // We only store&restore configuration events

		std::chrono::milliseconds TotalTrainingDuration = {};
		std::chrono::milliseconds CurrentTrainingStepDuration = {};
		std::chrono::milliseconds TimeSpentInTraining = {};
		std::chrono::milliseconds TimeLeftInProgram = {};
		std::chrono::milliseconds TimeLeftInCurrentTrainingStep = {};
	};
}