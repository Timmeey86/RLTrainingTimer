#pragma once

#include "../data/TrainingProgramData.h"

#include <map>
#include <memory>

#include <DLLImportExport.h>

namespace configuration
{
	/**
	 * The job of this class is to make sure that the list of training programs is always consistent (i.e. no double entries etc).
	 * It also does error checking of parameters, so rather than checking that everywhere, just place your UI layout function into a try catch block.
	 */
	class RLTT_IMPORT_EXPORT TrainingProgramListConfigurationControl
	{
	public:
		/** Creates an empty program list. */
		explicit TrainingProgramListConfigurationControl(std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData);

		/** Adds a new empty training program and returns its ID */
		uint64_t addTrainingProgram();
		
		/** Removes the training program with the given ID. */
		void removeTrainingProgram(uint64_t trainingProgramId);

		/** Swaps the positions of two training programs. */
		void swapTrainingPrograms(uint64_t firsttrainingProgramId, uint64_t secondtrainingProgramId);

		/** Provides a copy of the training program list data (e.g. for display). */
		TrainingProgramListData getTrainingProgramList() const;

		/** Provides a copy of data of a single training program (e.g. for display). */
		TrainingProgramData getTrainingProgramData(uint64_t trainingProgramId) const;

		/** Overrides any internal data. Use this only for restoring a saved state. */
		void restoreData(const TrainingProgramListData& data);

	private:
		void ensureIdDoesntExist(uint64_t trainingProgramId) const;
		void ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName) const;

		
		std::vector<uint64_t> _trainingProgramOrder; // The order of training programs
		std::shared_ptr<std::map<uint64_t, TrainingProgramData>> _trainingProgramData; // The training programs
	};
}