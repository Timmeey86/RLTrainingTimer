#include <pch.h>
#include "TrainingProgramRepository.h"
#include "uuid_generator.h"

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
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VarianceSettings, UseDefaultSettings, EnableTraining, LimitBoost, AllowMirror, PlayerVelocity, VarSpeed, VarLoc, VarLocZ, Shuffle, VarCarLoc, VarCarRot, VarSpin, VarRot);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramEntry, Name, Duration, Type, TrainingPackCode, WorkshopMapPath, TimeMode, Notes, Variance);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramData, Id, Name, Duration, Entries, ReadOnly);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrainingProgramListData, Version, TrainingProgramData, TrainingProgramOrder, WorkshopFolderLocation);

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

	void upgrade_from_1_1_to_1_2(json& deserialized)
	{
		LOG("Upgrading from version 1.1 to 1.2");
		auto& trainingProgramListData = deserialized["TrainingProgramData"];

		std::unordered_map<uint64_t, std::string> oldToNewIdMap;

		for (auto& trainingProgram : trainingProgramListData) {
			LOG("Reading training program data");
			auto& trainingProgramData = trainingProgram.at(1);
			LOG("Replacing ID by uuid");
			auto oldId = trainingProgramData["Id"].get<uint64_t>();
			auto newId = uuid_generator::generateUUID();
			trainingProgramData["Id"] = newId;
			trainingProgram.at(0) = newId;

			oldToNewIdMap.try_emplace(oldId, newId);
		}

		LOG("Converting former (int, data) array into (string, data) map");
		// Not sure why, but nlohmann::json seems to deserialized an int,data map into an array and a string,data map into a lookup object
		json newTrainingProgramListData;
		for (const auto& trainingProgram : trainingProgramListData) {
			auto trainingProgramData = trainingProgram.at(1);
			auto id = trainingProgramData["Id"].get<std::string>();
			newTrainingProgramListData[id] = trainingProgramData;
		}
		deserialized["TrainingProgramData"] = newTrainingProgramListData;

		LOG("Replacing IDs in training program oder");
		auto& trainingProgramOrder = deserialized["TrainingProgramOrder"];
		for (auto& entry : trainingProgramOrder)
		{
			auto oldId = entry.get<uint64_t>();
			auto newId = oldToNewIdMap.at(oldId);
			entry = newId;
		}

		LOG("Successfully upgraded to version 1.2");
	}

	void upgrade_from_1_2_to_1_3(json& deserialized)
	{
		LOG("Upgrading from version 1.2 to 1.3");

		// Add an entry for the new workshop folder location property
		deserialized["WorkshopFolderLocation"] = "";

		LOG("Successfully upgraded to version 1.3");
	}

	void upgrade_from_1_3_to_1_4(json& deserialized)
	{
		LOG("Upgrading from version 1.3 to 1.4");

		// Set all training programs to not be read-only
		auto& trainingProgramListData = deserialized["TrainingProgramData"];

		for (auto& trainingProgram : trainingProgramListData) {
			LOG("Patching training program data");
			trainingProgram["ReadOnly"] = false;
		}

		LOG("Successfully upgraded to version 1.4");
	}

	void upgrade_from_1_4_to_1_5(json& deserialized)
	{
		LOG("Upgrading from version 1.4 to 1.5");

		// Set all training programs to not be read-only
		auto& trainingProgramListData = deserialized["TrainingProgramData"];

		for (auto& trainingProgramData : trainingProgramListData) {
			LOG("Reading training program entries");
			auto& trainingProgramEntries = trainingProgramData["Entries"];
			LOG("Patching {} training program entries", trainingProgramEntries.size());
			for (auto& trainingProgramEntry : trainingProgramEntries)
			{
				// Add new fields, otherwise deserialization will fail
				trainingProgramEntry["TimeMode"] = configuration::TrainingProgramCompletionMode::Timed; // in 1.4, this was the only time mode available
				trainingProgramEntry["Notes"] = "";
				trainingProgramEntry["Variance"] = VarianceSettings();
			}
		}

		LOG("Successfully upgraded to version 1.5");
	}

	TrainingProgramListData TrainingProgramRepository::restoreData() const
	{
		if (!std::filesystem::exists(_storagePath)) { return {}; }

		std::ifstream is{ _storagePath };
		json deserialized;
		is >> deserialized;
		is.close();
	
		try
		{
			LOG("Trying to restore data");
			auto version = deserialized["Version"];
			LOG("Version is: {}", version);
			if (version == "1.0")
			{
				upgrade_from_1_0_to_1_1(deserialized);
			}
			if (version == "1.1")
			{
				upgrade_from_1_1_to_1_2(deserialized);
			}
			if (version == "1.2")
			{
				upgrade_from_1_2_to_1_3(deserialized);
			}
			if (version == "1.3")
			{
				upgrade_from_1_3_to_1_4(deserialized);
			}
			if (version == "1.4")
			{
				upgrade_from_1_4_to_1_5(deserialized);
				deserialized["Version"] = "1.5";
			}

			return deserialized.get<TrainingProgramListData>();
		}
		catch (json::type_error& err)
		{
			LOG("Failed restoring data for RLTrainingTimer: {}", err.what());
			return TrainingProgramListData();
		}
	}
}