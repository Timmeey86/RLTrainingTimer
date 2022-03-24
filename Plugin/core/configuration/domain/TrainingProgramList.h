#pragma once

#include "TrainingProgram.h"

#include <unordered_map>
#include <vector>
#include <memory>

namespace Core::Configuration::Domain
{
	/**
	 * This is an <<Aggregate>> which allows manipulating the list of available Training Programs, but not a Training Program itself
	 * 
	 * Entities:
	 *  - Training Programs
	 * Value Objects:
	 * Invariants:
	 *  - Every program only appears once in the list
	 *  - Every program ID only exists once
	 * 
	 * Throws:
	 *  - IndexOutOfBoundsException (Position)
	 *  - InvalidValueException (Training Program ID - e.g. if it exists already)
	 */
	class TrainingProgramList
	{
	public:
		/** Creates an empty program list. */
		TrainingProgramList() = default;

		/** Adds a new empty training program using the given ID. */
		void addTrainingProgram(uint64_t programId);
		
		/** Removes the training program with the given ID. */
		void removeTrainingProgram(uint64_t programId);

		/** Renames the training program with the given ID. */
		void renameTrainingProgram(uint64_t programId, const std::string& newName);

		/** Swaps the positions of two training programs. */
		void swapTrainingPrograms(uint64_t firstProgramId, uint64_t secondProgramId);

		/** Provides details for a training program so it can be edited. */
		std::shared_ptr<TrainingProgram> getTrainingProgram(uint64_t programId) const;

	private:
		std::vector<uint64_t> _trainingProgramOrder; // The order of training programs
		std::unordered_map<uint64_t, std::shared_ptr<TrainingProgram>> _trainingPrograms; // The training programs, indexable through their ID.
	};
}