#pragma once

#include <string>
#include <chrono>
#include <optional>

namespace training
{
	/** POD struct for data relevant for the UI which is displayed during actual training. */
	struct TrainingProgramExecutionData
	{
	public:
		/** If NumberOfSteps == 0, the program is considered invalid (there's nothing to be executed. The state of the other fields is undefined for this case.) */
		uint16_t NumberOfSteps;
		std::string TrainingProgramName;
		std::string TrainingStepName;
		uint16_t TrainingStepNumber;
		std::chrono::milliseconds DurationOfCurrentTrainingStep;
		std::chrono::milliseconds TimeLeftInCurrentTrainingStep;
		std::chrono::milliseconds TimeLeftInProgram;
		std::optional<std::chrono::steady_clock::time_point> TrainingFinishedTime; // The time at which the training had finished. This is useful for drawing something for a couple of seconds after finishing.
		bool TrainingIsPaused;
	};
}
