#pragma once

#include <string>
#include <chrono>

namespace configuration
{
	/** This allows figuring out which type of training program entry an entry is. */
	enum class TrainingProgramEntryType
	{
		Unspecified, // The user just assigns a label and a duration and figures out what to do himself
		Freeplay, // The user wants to do this training step in free play, and automatically load into freeplay if he's not in there
		CustomTraining // The user wants to specify a training pack code and load into that
	};


	class TrainingProgramEntry
	{
	public:
		std::string Name;
		std::chrono::milliseconds Duration;
		TrainingProgramEntryType Type = TrainingProgramEntryType::Unspecified;
		std::string TrainingPackCode; // Only set when Type = CustomTraining
	};
}

