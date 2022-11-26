#pragma once

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!!!! KEEP THIS FILE CONSISTENT WITH THE PREJUMP PLUGIN !!!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include "TrainingProgramEntry.h"

#include <DLLImportExport.h>

#include <vector>
#include <unordered_map>
#include <filesystem>

namespace configuration
{
	/** POD struct for data of a single training program. */
	struct TrainingProgramData
	{
	public:
		std::vector<TrainingProgramEntry> Entries;
		std::chrono::milliseconds Duration = {}; // Sum of entry durations. Stored separately since it changes rarely, but gets read often.
		std::string Id; // A uuid which uniquely identifies the training program. This uuid is shared with the prejump website!
		std::string Name;
		std::string Description;
		bool ReadOnly = false; // This is used so training programs from prejump can't be modified (but they can be copied and adapted, if desired)
	};

	/** POD struct for a list of training programs. */
	struct TrainingProgramListData
	{
	public:
		std::string Version = "1.5";
		std::string WorkshopFolderLocation = "";
		std::vector<std::string> TrainingProgramOrder;
		std::unordered_map<std::string, TrainingProgramData> TrainingProgramData;
	};

	/** This interface decouples file system handling from the remainder of the code, allowing easy changes like switching storage to a REST service
		instead of the file system, for example. Additionally, it allows easy testing of classes which rely on repository functionality by allowing mock or fake objects.
	*/
	class RLTT_IMPORT_EXPORT ITrainingProgramRepository
	{
	protected:
		ITrainingProgramRepository() = default;

	public:
		virtual ~ITrainingProgramRepository() = default;

		/** Stores the given training program list persistently at the default location. */
		virtual void storeData(const TrainingProgramListData& data) = 0;
		/** Stores the given training program list persistently at the specified location. */
		virtual void storeData(const TrainingProgramListData& data, const std::string& location) = 0;
		/** Restores the training program list from the default persistent location. */
		virtual TrainingProgramListData restoreData() const = 0;
		/** Restores the training program list from the specified persistent location. */
		virtual TrainingProgramListData restoreData(const std::string& location) const = 0;

		/** Exports a single training program persistently at the specified location in order to allow the user to share it with others. */
		virtual void exportSingleTrainingProgram(const TrainingProgramData& data, const std::string& location) = 0;
		/** Imports a single training program from the specified persistent location. */
		virtual TrainingProgramData importSingleTrainingProgram(const std::string& location) const = 0;
	};

	/** Simple interface for classes which consume training programs. */
	class RLTT_IMPORT_EXPORT ITrainingProgramListReceiver
	{
	protected:
		ITrainingProgramListReceiver() = default;

	public:
		virtual ~ITrainingProgramListReceiver() = default;

		/** Populates the receiver with the given list of training programs. */
		virtual void receiveListData(const TrainingProgramListData& data) = 0;
	};
}