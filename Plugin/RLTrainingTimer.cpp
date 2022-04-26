#include <pch.h>
#include "RLTrainingTimer.h"

#include "ui/training/TrainingProgramFlowControlUi.h"
#include "ui/training/TrainingProgramDisplay.h"


BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;

	// Allow defining training programs
	auto trainingProgramDataMap = std::make_shared<std::map<uint64_t, configuration::TrainingProgramData>>();
	auto singleTrainingProgramControl = std::make_shared<configuration::TrainingProgramConfigurationControl>(trainingProgramDataMap);
	auto trainingProgramListControl = std::make_shared<configuration::TrainingProgramListConfigurationControl>(trainingProgramDataMap);
	initConfigurationUi(gameWrapper, singleTrainingProgramControl, trainingProgramListControl);

	//// Connect the configuration UI and the configuration service
	//connectToAppService(_configurationAppService, gameWrapper);

	//// Allow storing and restoring training programs
	//_configurationPersistenceService = std::make_shared<Core::Configuration::Application::TrainingProgramPersistenceService>(
	//	gameWrapper,
	//	[this](const std::vector<std::shared_ptr<Core::Kernel::DomainEvent>>& events) { _configurationAppService->applyEvents(events); }
	//);
	//_configurationAppService->registerEventReceiver(_configurationPersistenceService.get());

	//// Set up a service for actual training
	//_trainingAppService = std::make_shared<Core::Training::Application::TrainingApplicationService>();
	//_eventAdapter = std::make_shared<Adapter::RocketLeagueEventAdapter>(_trainingAppService);
	//_eventAdapter->hookToEvents(gameWrapper);

	//// Initialize the UI to be displayed when training
	//initTrainingProgramFlowControlUi(gameWrapper, _trainingAppService);

	//// Connect the configuration domain (managing training programs) with the training domain (executing training programs)
	//_configurationAppService->registerEventReceiver(_trainingAppService.get());

	//// Now that everything was set up, restore the stored events (if available)
	//_configurationPersistenceService->restoreEvents();

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}