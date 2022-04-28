#pragma once

#include "TrainingProgramEntry.h"

#include <DLLImportExport.h>

#include <vector>
#include <unordered_map>

namespace configuration
{
	/** POD struct for data of a single training program. */
	struct TrainingProgramData
	{
	public:
		std::vector<TrainingProgramEntry> Entries;
		std::chrono::milliseconds Duration = {}; // Sum of entry durations. Stored separately since it changes rarely, but gets read often.
		uint64_t Id;
		std::string Name;
	};

	/** POD struct for a list of training programs. */
	struct TrainingProgramListData
	{
	public:
		std::string Version = "1.0";
		std::vector<uint64_t> TrainingProgramOrder;
		std::unordered_map<uint64_t, TrainingProgramData> TrainingProgramData;
	};

	/** This interface allows testing classes which want to store and restore training programs without actually having to write to the file system. */
	class RLTT_IMPORT_EXPORT ITrainingProgramRepository
	{
	protected:
		ITrainingProgramRepository() = default;

	public:
		virtual ~ITrainingProgramRepository() = default;

		/** Stores the given training program list persistently. */
		virtual void storeData(const TrainingProgramListData& data) = 0;
		/** Restores the training program list from the persistent location. */
		virtual TrainingProgramListData restoreData() const = 0;
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