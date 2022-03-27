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
		uint32_t TrainingProgramDuration; // ms
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
		std::shared_ptr<Kernel::DomainEvent> addTrainingProgram(uint64_t trainingProgramId);
		
		/** Removes the training program with the given ID. */
		std::shared_ptr<Kernel::DomainEvent> removeTrainingProgram(uint64_t trainingProgramId);

		/** Renames the training program with the given ID. */
		std::shared_ptr<Kernel::DomainEvent> renameTrainingProgram(uint64_t trainingProgramId, const std::string& newName);

		/** Swaps the positions of two training programs. */
		std::shared_ptr<Kernel::DomainEvent> swapTrainingPrograms(uint64_t firsttrainingProgramId, uint64_t secondtrainingProgramId);

		/** Applies the given list of events to this object. */
		void applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events);

		/** Provides details for a training program so it can be edited. */
		std::shared_ptr<TrainingProgram> getTrainingProgram(uint64_t trainingProgramId) const;

		/** Provides high-level information about the programs managed by this class. */
		std::vector<TrainingProgramListEntry> getListEntries() const;

	private:
		void ensureIdDoesntExist(uint64_t trainingProgramId);
		void ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName);
		
		std::vector<uint64_t> _trainingProgramOrder; // The order of training programs
		std::unordered_map<uint64_t, std::shared_ptr<TrainingProgram>> _trainingPrograms; // The training programs, indexable through their ID.
	};
}