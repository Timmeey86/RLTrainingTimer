#include <pch.h>
#include "TrainingProgramRepository.h"

#include <external/nlohmann/json.hpp>

#include <ostream>
#include <fstream>

using json = nlohmann::json;

// Allow serialization of std::chrono::milliseconds
namespace nlohmann {
	template <>
	struct adl_serializer<std::chrono::milliseconds> {
		static void to_json(json& j, const std::chrono::milliseconds& ms) { j = ms.count(); }
		static void from_json(const json& j, std::chrono::milliseconds& ms) { ms = std::chrono::milliseconds(j); }
	};
}

namespace configuration
{
	// Allow serialization of training programs
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramEntry, Name, Duration, Type, TrainingPackCode, WorkshopMapPath);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramData, Id, Name, Duration, Entries);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramListData, Version, TrainingProgramData, TrainingProgramOrder);

	TrainingProgramRepository::TrainingProgramRepository(const std::shared_ptr<GameWrapper>& gameWrapper)
	{
		auto configurationFolderPath = gameWrapper->GetDataFolder() / "RLTrainingTimer";
		_storagePath = configurationFolderPath / "trainingprogramlist.json";
		if (!std::filesystem::exists(configurationFolderPath))
		{
			std::filesystem::create_directories(configurationFolderPath);
		}
	}
	void TrainingProgramRepository::storeData(const TrainingProgramListData& data)
	{
		json serialized = data;
		std::ofstream os{ _storagePath };
		os << serialized.dump(2);
		os.flush();
	}


	void upgrade_from_1_0_to_1_1(json& deserialized)
	{
		LOG("Upgrading from version 1.0 to 1.1");
		auto& trainingProgramListData = deserialized["TrainingProgramData"];

		for (auto& trainingProgram : trainingProgramListData) {
			LOG("Reading training program data");
			auto& trainingProgramData = trainingProgram.at(1);
			LOG("Reading training program entries");
			auto& trainingProgramEntries = trainingProgramData["Entries"];
			LOG("Patching {} training program entries", trainingProgramEntries.size());
			for (auto& trainingProgramEntry : trainingProgramEntries)
			{
				// Add new fields, otherwise deserialization will fail
				trainingProgramEntry["Type"] = 0;
				trainingProgramEntry["TrainingPackCode"] = std::string();
				trainingProgramEntry["WorkshopMapPath"] = std::string();
			}
		}
		LOG("Successfully upgraded to version 1.1");
	}

	TrainingProgramListData TrainingProgramRepository::restoreData() const
	{
		if (!std::filesystem::exists(_storagePath)) { return {}; }

		std::ifstream is{ _storagePath };
		json deserialized;
		is >> deserialized;
		is.close();
	
		LOG("Trying to restore data");
		auto version = deserialized["Version"];
		LOG("Version is: {}", version);
		if (version == "1.0")
		{
			upgrade_from_1_0_to_1_1(deserialized);
		}

		return deserialized.get<TrainingProgramListData>();
	}
}