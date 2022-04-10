#include <pch.h>
#include "RLTrainingTimer.h"


BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;

	// Connect the configuration UI and the configuration service
	connectToAppService(_configurationAppService);

	// Allow storing and restoring training programs
	_configurationPersistenceService = std::make_shared<Core::Configuration::Application::TrainingProgramPersistenceService>(
		gameWrapper,
		[this](const std::vector<std::shared_ptr<Core::Kernel::DomainEvent>>& events) { _configurationAppService->applyEvents(events); }
	);
	_configurationAppService->registerEventReceiver(_configurationPersistenceService.get());

	// Set up a service for actual training
	_trainingAppService = std::make_shared<Core::Training::Application::TrainingApplicationService>();
	_eventAdapter = std::make_shared<Adapter::RocketLeagueEventAdapter>(_trainingAppService);
	_eventAdapter->hookToEvents(gameWrapper);

	// Initialize the UI to be displayed when training
	initTrainingProgramFlowControlUi(gameWrapper, _trainingAppService);

	// Connect the configuration domain (managing training programs) with the training domain (executing training programs)
	_configurationAppService->registerEventReceiver(_trainingAppService.get());

	// Now that everything was set up, restore the stored events (if available)
	_configurationPersistenceService->restoreEvents();

	//// DUMMY CODE
	//_configurationAppService->addTrainingProgram({});
	//_configurationAppService->addTrainingProgram({});
	//_configurationAppService->addTrainingProgram({});

	//_configurationAppService->renameTrainingProgram({ 1, "Coaching Training Schedule" });
	//_configurationAppService->renameTrainingProgram({ 2, "Demo Program" });
	//_configurationAppService->renameTrainingProgram({ 3, "Third Program" });

	//_configurationAppService->addTrainingProgramEntry({ 1 });
	//_configurationAppService->addTrainingProgramEntry({ 1 });
	//_configurationAppService->addTrainingProgramEntry({ 1 });
	//_configurationAppService->addTrainingProgramEntry({ 1 });
	//_configurationAppService->addTrainingProgramEntry({ 1 });
	//_configurationAppService->addTrainingProgramEntry({ 1 });

	//_configurationAppService->renameTrainingProgramEntry({ 1, 0, "Forward Wave Dashes (No Boost)" });
	//_configurationAppService->renameTrainingProgramEntry({ 1, 1, "Forward Wave Dashes (With Boost)" });
	//_configurationAppService->renameTrainingProgramEntry({ 1, 2, "Forward Wave Dashes (Ball Cam On)" });
	//_configurationAppService->renameTrainingProgramEntry({ 1, 3, "Any Direction Wave Dashes" });
	//_configurationAppService->renameTrainingProgramEntry({ 1, 4, "Half Flips (ball cam off)" });
	//_configurationAppService->renameTrainingProgramEntry({ 1, 5, "Saves from the Wall (use BM)" });

	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 0, std::chrono::minutes(2) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 1, std::chrono::minutes(3) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 2, std::chrono::minutes(3) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 3, std::chrono::minutes(2) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 4, std::chrono::minutes(5) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 1, 5, std::chrono::minutes(5) });

	//_configurationAppService->addTrainingProgramEntry({ 2 });
	//_configurationAppService->addTrainingProgramEntry({ 2 });
	//_configurationAppService->addTrainingProgramEntry({ 2 });
	//_configurationAppService->renameTrainingProgramEntry({ 2, 0, "Wave Dashes" });
	//_configurationAppService->renameTrainingProgramEntry({ 2, 1, "Step with a very long name which needs reduced font size" });
	//_configurationAppService->renameTrainingProgramEntry({ 2, 2, "Ground Dribbling" });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 2, 0, std::chrono::seconds(8) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 2, 1, std::chrono::seconds(8) });
	//_configurationAppService->changeTrainingProgramEntryDuration({ 2, 2, std::chrono::seconds(8) });

	//_configurationAppService->addTrainingProgramEntry({ 3 });

	//_configurationPersistenceService->storeAllEvents();

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}