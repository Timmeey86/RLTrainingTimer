#pragma once

#include <string>

namespace Core::Configuration::Domain
{
	/**
	 * This is a <<Value Object>> which represents a single entry in a training program.
	 * Rather than changing the entry, it will get replaced by a new one
	 */
	class TrainingProgramEntry
	{
	public:
		/** Constructs a new training program entry. */
		TrainingProgramEntry(std::string name, uint32_t duration)
			: _name{ name }
			, _duration{ duration }
		{}

		/** Retrieves the name of the entry. */
		inline std::string name() const { return _name; }
		/** Retrieves the duration of the entry. */
		inline uint32_t duration() const { return _duration; }

	private:
		std::string _name;
		uint32_t _duration;
	};
}

