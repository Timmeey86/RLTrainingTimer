#pragma once

#include "../data/TrainingProgramData.h"
#include "ITrainingProgramListReceiver.h"

#include <map>
#include <memory>
#include <functional>

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
		TrainingProgramListConfigurationControl(
			std::shared_ptr<std::map<std::string, TrainingProgramData>> trainingProgramData,
			std::shared_ptr<ITrainingProgramRepository> repository
			);

		/** Registers a receiver for training program list changes. */
		void registerTrainingProgramListReceiver(std::shared_ptr<ITrainingProgramListReceiver> receiver);

		/** Adds a new empty training program and returns its uuid */
		std::string addTrainingProgram();
		
		/** Removes the training program with the given ID. */
		void removeTrainingProgram(std::string trainingProgramId);

		/** Swaps the positions of two training programs. */
		void swapTrainingPrograms(std::string firsttrainingProgramId, std::string secondtrainingProgramId);

		/** Changes the location to the workshop folder. */
		void changeWorkshopFolderLocation(const std::string& newLocation);

		/** Creates or replaces a training program, supplied from an external source. */
		void injectTrainingProgram(const TrainingProgramData& data);

		/** Checks if a training program exists. */
		inline bool hasTrainingProgram(const std::string& uuid) const { return _trainingProgramData->count(uuid) > 0; }

		/** Provides a copy of the training program list data (e.g. for display). */
		TrainingProgramListData getTrainingProgramList() const;

		/** Provides a copy of data of a single training program (e.g. for display). */
		TrainingProgramData getTrainingProgramData(std::string trainingProgramId) const;

		/** Restores data from the repository. */
		void restoreData();

		/** Notifies any receiver about a change in the training program list. */
		void notifyReceivers(bool currentlyRestoringData = false);

	private:
		void ensureIdDoesntExist(std::string trainingProgramId) const;
		void ensureIdIsKnown(std::string trainingProgramId, const std::string& parameterName) const;
		
		std::string _workshopFolderLocation = ""; // The location of the workshop maps folder
		std::vector<std::string> _trainingProgramOrder; // The order of training programs
		std::shared_ptr<std::map<std::string, TrainingProgramData>> _trainingProgramData; // The training programs
		std::vector<std::shared_ptr<ITrainingProgramListReceiver>> _receivers;
		std::shared_ptr<ITrainingProgramRepository> _repository; // Allows storing training programs.
	};
}