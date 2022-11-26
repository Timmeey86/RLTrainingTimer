#pragma once

#include <external/nlohmann/json.hpp>

using json = nlohmann::json;

namespace configuration
{
	/** This class is responsible for reading contents from older file formats and patching them to the most recent version.
	    This allows the repository to always work on up-to-date data, thus not requiring any version-dependent switches. */
	class TrainingProgramDataPatcher
	{
	public:
		static json patchData(json deserialized);
	};
}