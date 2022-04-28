#pragma once

#include <string>
#include <chrono>

namespace configuration
{
	/**
	 * This is a <<Value Object>> which represents a single entry in a training program.
	 */
	class TrainingProgramEntry
	{
	public:
		std::string Name;
		std::chrono::milliseconds Duration;
	};
}

