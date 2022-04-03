#pragma once

#include <string>
#include <chrono>
namespace Core::Configuration::Domain
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

