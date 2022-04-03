#pragma once

#include "TrainingProgram.h"

#include <unordered_map>
#include <vector>
#include <memory>

#include <DLLImportExport.h>

namespace Core::Configuration::Domain
{
	/** Provides high-level information about a program managed by TrainingProgramList. */
	struct TrainingProgramListEntry
	{
	public:
		uint64_t TrainingProgramId;
		std::string TrainingProgramName;
		std::chrono::milliseconds TrainingProgramDuration;
	};
	/**
	 * This is an <<Aggregate Root>> <<Entity>> which allows manipulating the list of available Training Programs, but not a Training Program itself
	 * 
	 * Entities:
	 *  - Training Programs
	 * Value Objects:
	 * Invariants:
	 *  - Every program only appears once in the list
	 *  - Every program ID only exists once
	 * 
	 * Throws:
	 *  - InvalidValueException (Training Program ID - e.g. if it exists already)
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramList
	{
	public:
		/** Creates an empty program list. */
		TrainingProgramList() = default;

		/** Adds a new empty training program using the given ID. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> addTrainingProgram(uint64_t trainingProgramId);
		
		/** Removes the training program with the given ID. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> removeTrainingProgram(uint64_t trainingProgramId);

		/** Swaps the positions of two training programs. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> swapTrainingPrograms(uint64_t firsttrainingProgramId, uint64_t secondtrainingProgramId);

		/** Applies the given list of events to this object. */
		void applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events);

		/** Provides details for a training program so it can be edited. */
		std::shared_ptr<TrainingProgram> getTrainingProgram(uint64_t trainingProgramId) const;

		/** Creates an event which publishes the new training program list. This can be used to easily create such an event when e.g. the duration of a training program changes. */
		std::shared_ptr<Kernel::DomainEvent> createListChangedEvent();

		/** Helper which creates a new ListChangedEvent and adds it to a list of already existing events. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> addListChangedEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>> otherEvents);

		/** Retrieves the maximum ID used by any of the training programs right now. */
		uint64_t getMaximumId() const;

	private:
		void ensureIdDoesntExist(uint64_t trainingProgramId) const;
		void ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName) const;

		
		std::vector<uint64_t> _trainingProgramOrder; // The order of training programs
		std::unordered_map<uint64_t, std::shared_ptr<TrainingProgram>> _trainingPrograms; // The training programs, indexable through their ID.
	};
}