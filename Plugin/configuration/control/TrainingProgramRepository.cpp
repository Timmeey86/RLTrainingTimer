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
		storeDataImpl(data, _storagePath);
	}

	void TrainingProgramRepository::storeData(const TrainingProgramListData& data, const std::string& location)
	{
		storeDataImpl(data, std::filesystem::path(location));
	}

	void TrainingProgramRepository::storeDataImpl(const TrainingProgramListData& data, const std::filesystem::path& path)
	{
		json serialized = data;
		std::ofstream os{ path };
		os << serialized.dump(2);
		os.flush();
	}


	TrainingProgramListData TrainingProgramRepository::restoreData() const
	{
		return restoreDataImpl(_storagePath);
	}

	TrainingProgramListData TrainingProgramRepository::restoreData(const std::string& location) const
	{
		return restoreDataImpl(std::filesystem::path(location));
	}

	TrainingProgramListData TrainingProgramRepository::restoreDataImpl(const std::filesystem::path& path) const
	{
		if (!std::filesystem::exists(path)) { return {}; }

		std::ifstream is{ path };
		json deserialized;
		is >> deserialized;
		is.close();

		try
		{
			return deserialized.get<TrainingProgramListData>();
		}
		catch (json::type_error& err)
		{
			LOG("Failed restoring data for RLTrainingTimer: {}", err.what());
			return TrainingProgramListData();
		}
	}
	void TrainingProgramRepository::exportSingleTrainingProgram(const TrainingProgramData& data, const std::string& location)
	{
		LOG("Save Training program {}..", data.Id);

		// serialize the training program to json
		json jsonData = data;

		// write the json to the path
		auto path = std::filesystem::path(location);
		std::ofstream os{ path };
		os << jsonData.dump(2);
		os.flush();
	}

	TrainingProgramData TrainingProgramRepository::importSingleTrainingProgram(const std::string& location) const
	{
		LOG("Load Training program..");

		// load the full contents of the file into a string
		std::string serialized;
		std::stringstream buffer;

		auto path = std::filesystem::path(location);
		if (!std::filesystem::exists(path)) {
			LOG("File does not exist");
			return {};
		}

		std::ifstream is{ path };
		buffer << is.rdbuf();
		is.close();
		serialized = buffer.str();

		// try to parse it as json
		json jsonData;
		try
		{
			jsonData = json::parse(serialized);
		}
		catch (json::exception ex)
		{
			LOG("Could not parse JSON Data: {}", ex.what());
			LOG("ERROR: Data in file was: {}", serialized);
			return {};
		}

		// try to parse as training program
		try
		{
			return jsonData.get<configuration::TrainingProgramData>();
		}
		catch (json::exception ex)
		{
			LOG("JSON Data does not match training program structure: {}", ex.what());
			LOG("ERROR: JSON Data was: {}", jsonData.dump(2));
			return {};
		}
	}
}