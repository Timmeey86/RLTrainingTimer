#pragma once


#include "TrainingProgramEntry.h"

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
}