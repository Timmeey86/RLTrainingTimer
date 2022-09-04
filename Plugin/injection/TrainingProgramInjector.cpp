#include <pch.h>
#include "TrainingProgramInjector.h"
#include <configuration/data/TrainingProgramData.h>
#include <external/nlohmann/json.hpp>
#include <algorithm>

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
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configuration::TrainingProgramEntry, Name, Duration, Type, TrainingPackCode, WorkshopMapPath);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configuration::TrainingProgramData, Id, Name, Duration, Entries, ReadOnly);
}

namespace injection
{

	const std::string TrainingProgramInjector::InjectProgramNotifier = "rltt_inject_training_program";
	const std::string TrainingProgramInjector::RunProgramNotifier = "rltt_run_training_program";
	const std::string TrainingProgramInjector::InjectAndRunProgramNotifier = "rltt_inject_and_run_training_program";

	TrainingProgramInjector::TrainingProgramInjector(
		std::shared_ptr<CVarManagerWrapper> cvarManager,
		std::shared_ptr<configuration::TrainingProgramListConfigurationControl> programListControl,
		std::shared_ptr<training::TrainingProgramFlowControl> programFlowControl,
		training::IErrorDisplay* errorDisplay)
		: _cvarManager{ std::move(cvarManager) }
		, _programListControl{ std::move(programListControl) }
		, _programFlowControl{ std::move(programFlowControl) }
		, _errorDisplay{ errorDisplay }
	{

	}
	void TrainingProgramInjector::registerNotifiers()
	{
		_cvarManager->registerNotifier(InjectProgramNotifier, [this](const std::vector<std::string>& params) {
			receiveTrainingProgram(params);
		}, "", PERMISSION_ALL);

		_cvarManager->registerNotifier(RunProgramNotifier, [this](const std::vector<std::string>& params) {
			if (params.size() != 2)
			{
				_errorDisplay->displayErrorMessage("Invalid syntax", fmt::format("Syntax is : {} <uuid>", RunProgramNotifier));
				return;
			}
			executeTrainingProgram(params[1]);
		}, "", PERMISSION_ALL);

		_cvarManager->registerNotifier(InjectAndRunProgramNotifier, [this](const std::vector<std::string>& params) {
			auto uuid = receiveTrainingProgram(params);
			if (uuid.empty())
			{
				return; // Log message has already been shown
			}
			executeTrainingProgram(uuid);
		}, "", PERMISSION_ALL);

		_cvarManager->registerNotifier(InjectAndRunProgramNotifier, [this](const std::vector<std::string>& params) {
			auto uuid = receiveTrainingProgram(params);
			if (uuid.empty())
			{
				return; // Log message has already been shown
			}
			executeTrainingProgram(uuid);
			}, "", PERMISSION_ALL);
	}

	std::string TrainingProgramInjector::receiveTrainingProgram(const std::vector<std::string>& params)
	{
		_errorDisplay->clearErrorMessages();

		if (params.size() != 2)
		{
			_errorDisplay->displayErrorMessage("Invalid syntax", fmt::format("Syntax is : {} <json_data>", InjectProgramNotifier));
			return std::string();
		}

		json jsonData;
		try
		{
			jsonData = json::parse(params[1]);
		}
		catch (json::exception ex)
		{
			_errorDisplay->displayErrorMessage("Could not parse JSON Data", ex.what());
			return std::string();
		}

		configuration::TrainingProgramData trainingProgramData;
		try
		{
			trainingProgramData = jsonData.get<configuration::TrainingProgramData>();
		}
		catch (json::exception ex)
		{
			_errorDisplay->displayErrorMessage("JSON Data does not match training program structure", ex.what());
			LOG("ERROR: JSON Data was: {}", jsonData.dump(2));
			return std::string();
		}

		LOG("Successfully deserialized training program. Trying to inject it.");
		_programListControl->injectTrainingProgram(trainingProgramData);
		return trainingProgramData.Id;
	}

	void TrainingProgramInjector::executeTrainingProgram(const std::string& uuid) const
	{
		if (!_programListControl->hasTrainingProgram(uuid))
		{
			_errorDisplay->displayErrorMessage("Unknown training program", fmt::format("Training program UUID {} is not known", uuid));
			return;
		}

		_programFlowControl->stopRunningTrainingProgram(); // In case something is running.. Will do nothing if not
		_programFlowControl->selectTrainingProgram(uuid);
		_programFlowControl->startSelectedTrainingProgram();
	}
}