#pragma once

#include "../data/TrainingProgramData.h"

#include <map>
#include <memory>

#include <DLLImportExport.h>

namespace configuration
{
	/**
	 * The job of this class is to make sure that the TrainingProgramData structs are always consistent.
	 * It also does error checking of parameters, so rather than checking that everywhere, just place your UI layout function into a try catch block.
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramConfigurationControl
	{
	public:
		/** Constructor. */
		explicit TrainingProgramConfigurationControl(std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData);

		/** Adds an entry to the training program. */
		void addEntry(uint64_t trainingProgramId, const TrainingProgramEntry& entry);

		/** Removes the entry at the given position from the training program. */
		void removeEntry(uint64_t trainingProgramId, int position);

		/** Renames the entry at the given position. */
		void renameEntry(uint64_t trainingProgramId, int position, const std::string& newName);

		/** Changes the duration of the entry at the given position. */
		void changeEntryDuration(uint64_t trainingProgramId, int position, const std::chrono::milliseconds& newDuration);

		/** Swaps the positions of two entries. */
		void swapEntries(uint64_t trainingProgramId, int firstPosition, int secondPosition);

		/** Changes the name of the training program. */
		void renameProgram(uint64_t trainingProgramId, const std::string& newName);

		/** Retrieves a copy of the training program data for the given ID. */
		TrainingProgramData getData(uint64_t trainingProgramId) const;

	private:

		TrainingProgramData* internalData(uint64_t trainingProgramId) const;

		void validatePosition(const TrainingProgramData* const data, int position, const std::string& variableName) const;

		std::shared_ptr<std::map<uint64_t, TrainingProgramData>> _trainingProgramData;

	};
}