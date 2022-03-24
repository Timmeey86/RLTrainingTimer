#pragma once

#include "TrainingProgramEntry.h"
#include "../events/TrainingProgramEntryEvents.h"
#include "../events/TrainingProgramEvents.h"

#include <vector>
#include <queue>

#include <DLLImportExport.h>

namespace Core::Configuration::Domain
{
	/**
	 * This is a simplified <<Aggregate>> where the root <<Entity>> is the aggregate itself.
	 * It represents a single training program, consisting of several training program entries.
	 *
	 * Entities:
	 *  - None in addition to itself
	 * Value Objects:
	 *  - List of Training Program Entries
	 * Invariants:
	 *  - Training Program Duration = Sum of Training Entry durations
	 *
	 * Throws:
	 *  - IndexOutOfBoundsException (Position)
	 */
	class RLTT_IMPORT_EXPORT TrainingProgram
	{
	public:
		/** Creates an empty training program. */
		TrainingProgram(uint64_t id);

		/** Adds an entry to the training program. */
		Events::TrainingProgramEntryAddedEvent addEntry(const TrainingProgramEntry& entry);

		/** Removes the entry at the given position from the training program. */
		Events::TrainingProgramEntryRemovedEvent removeEntry(int position);

		/** Replaces the entry at the given position with the new entry. */
		Events::TrainingProgramEntryUpdatedEvent replaceEntry(int position, const TrainingProgramEntry& newEntry);

		/** Swaps the positions of two entries. */
		Events::TrainingProgramEntrySwappedEvent swapEntries(int firstPosition, int secondPosition);

		/** Changes the name of the training program. */
		Events::TrainingProgramRenamedEvent renameProgram(const std::string& newName);
		
		/** Re-creates a training program based on events which had occurred on it in the past. */
		void loadFromEvents(const std::vector<Kernel::DomainEvent>& events);

		/** Retrieves the total program duration. */
		uint32_t programDuration() const;

		/** Retrieves the unique ID of the training program. */
		uint64_t id() const;
		
		/** Retrieves the name of the training program. */
		std::string name() const;

	private:

		void validatePosition(int position, const std::string& variableName) const;

		std::vector<TrainingProgramEntry> _entries;
		uint32_t _duration = 0; // Note: UInt32 will allow for up to 49 days of training. This must be enough ;)
		uint64_t _id;
		std::string _name;
	};
}