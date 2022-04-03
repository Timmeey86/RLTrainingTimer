#pragma once

#include "TrainingProgramEntry.h"
#include <core/kernel/DomainEvent.h>

#include <vector>
#include <queue>

#include <DLLImportExport.h>

namespace Core::Configuration::Domain
{
	/**
	 * This is an <<Entity>> used as <<Aggregate Root>>.
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
		explicit TrainingProgram(uint64_t id);

		/** Adds an entry to the training program. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> addEntry(const TrainingProgramEntry& entry);

		/** Removes the entry at the given position from the training program. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> removeEntry(int position);

		/** Renames the entry at the given position. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> renameEntry(int position, const std::string& newName);

		/** Changes the duration of the entry at the given position. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> changeEntryDuration(int position, const std::chrono::milliseconds& newDuration);

		/** Swaps the positions of two entries. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> swapEntries(int firstPosition, int secondPosition);

		/** Changes the name of the training program. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> renameProgram(const std::string& newName);

		/** Applies the given list of events to this object. */
		void applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events);

		/** Helper which creates a new ProgramChangedEvent and adds it to a list of already existing events. */
		std::vector<std::shared_ptr<Kernel::DomainEvent>> addProgramChangedEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>> otherEvents);

		/** Retrieves the total program duration. */
		std::chrono::milliseconds programDuration() const;

		/** Retrieves the unique ID of the training program. */
		uint64_t id() const;

		/** Retrieves the name of the training program. */
		std::string name() const;

		/** Retrieves a deep copy of the entries in this training program. */
		std::vector<TrainingProgramEntry> entries() const;

		/** Retrieves the amount of entries in this training program. */
		inline uint16_t entryCount() const { return (uint16_t)_entries.size(); }

	private:

		void validatePosition(int position, const std::string& variableName) const;

		std::vector<TrainingProgramEntry> _entries;
		std::chrono::milliseconds _duration = {};
		uint64_t _id;
		std::string _name;
	};
}