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
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramEntry, Name, Duration);
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
	TrainingProgramListData TrainingProgramRepository::restoreData() const
	{
		if (!std::filesystem::exists(_storagePath)) { return {}; }

		std::ifstream is{ _storagePath };
		json deserialized;
		is >> deserialized;
		is.close();

		// TODO abort if version is not 1.0
		return deserialized.get<TrainingProgramListData>();
	}
}