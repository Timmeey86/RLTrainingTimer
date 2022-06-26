#pragma once

#include "../data/TrainingProgramData.h"

#include <map>
#include <memory>
#include <functional>

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
		explicit TrainingProgramConfigurationControl(
			std::shared_ptr<std::map<std::string, TrainingProgramData>> trainingProgramData,
			std::function<void()> changeNotificationCallback
		);

		/** Adds an entry to the training program. */
		void addEntry(const std::string& trainingProgramId, const TrainingProgramEntry& entry);

		/** Removes the entry at the given position from the training program. */
		void removeEntry(const std::string& trainingProgramId, int position);

		/** Renames the entry at the given position. */
		void renameEntry(const std::string& trainingProgramId, int position, const std::string& newName);

		/** Changes the duration of the entry at the given position. */
		void changeEntryDuration(const std::string& trainingProgramId, int position, const std::chrono::milliseconds& newDuration);

		/** Swaps the positions of two entries. */
		void swapEntries(const std::string& trainingProgramId, int firstPosition, int secondPosition);

		/** Changes the type of the training program entry. */
		void changeEntryType(const std::string& trainingProgramId, int position, TrainingProgramEntryType type);

		/** Changes the training pack code of the training program entry. */
		void changeTrainingPackCode(const std::string& trainingProgramId, int position, const std::string& trainingPackCode);

		/** Changes the path to the workshop map for a training step. */
		void changeWorkshopMapPath(const std::string& trainingProgramId, int position, const std::string& workshopMapPath);

		/** Changes the name of the training program. */
		void renameProgram(const std::string& trainingProgramId, const std::string& newName);

		/** Retrieves a copy of the training program data for the given ID. */
		TrainingProgramData getData(const std::string& trainingProgramId) const;

	private:

		TrainingProgramData* internalData(const std::string& trainingProgramId) const;

		void validatePosition(const TrainingProgramData* const data, int position, const std::string& variableName) const;

		std::shared_ptr<std::map<std::string, TrainingProgramData>> _trainingProgramData;
		std::function<void()> _changeNotificationCallback;

	};
}