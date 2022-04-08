#include <pch.h>
#include "RLTrainingTimer.h"


BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;

	connectToAppService(_configurationAppService);

	_trainingAppService = std::make_shared<Core::Training::Application::TrainingApplicationService>();

	_eventAdapter = std::make_shared<Adapter::RocketLeagueEventAdapter>(_trainingAppService);
	_eventAdapter->hookToEvents(gameWrapper);

	initTrainingProgramFlowControlUi(gameWrapper, _trainingAppService);
	_configurationAppService->registerEventReceiver(_trainingAppService.get());





	// DUMMY CODE
	_configurationAppService->addTrainingProgram({});
	_configurationAppService->addTrainingProgram({});
	_configurationAppService->addTrainingProgram({});

	_configurationAppService->renameTrainingProgram({ 1, "Coaching Training Schedule" });
	_configurationAppService->renameTrainingProgram({ 2, "Demo Program" });
	_configurationAppService->renameTrainingProgram({ 3, "Third Program" });

	_configurationAppService->addTrainingProgramEntry({ 1 });
	_configurationAppService->addTrainingProgramEntry({ 1 });
	_configurationAppService->addTrainingProgramEntry({ 1 });
	_configurationAppService->addTrainingProgramEntry({ 1 });
	_configurationAppService->addTrainingProgramEntry({ 1 });
	_configurationAppService->addTrainingProgramEntry({ 1 });

	_configurationAppService->renameTrainingProgramEntry({ 1, 0, "Forward Wave Dashes (No Boost)" });
	_configurationAppService->renameTrainingProgramEntry({ 1, 1, "Forward Wave Dashes (With Boost)" });
	_configurationAppService->renameTrainingProgramEntry({ 1, 2, "Forward Wave Dashes (Ball Cam On)" });
	_configurationAppService->renameTrainingProgramEntry({ 1, 3, "Any Direction Wave Dashes" });
	_configurationAppService->renameTrainingProgramEntry({ 1, 4, "Half Flips (ball cam off)" });
	_configurationAppService->renameTrainingProgramEntry({ 1, 5, "Saves from the Wall (use BM)" });

	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 0, std::chrono::minutes(2) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 1, std::chrono::minutes(3) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 2, std::chrono::minutes(3) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 3, std::chrono::minutes(2) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 4, std::chrono::minutes(5) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 1, 5, std::chrono::minutes(5) });

	_configurationAppService->addTrainingProgramEntry({ 2 });
	_configurationAppService->addTrainingProgramEntry({ 2 });
	_configurationAppService->addTrainingProgramEntry({ 2 });
	_configurationAppService->renameTrainingProgramEntry({ 2, 0, "Wave Dashes" });
	_configurationAppService->renameTrainingProgramEntry({ 2, 1, "Half Flips" });
	_configurationAppService->renameTrainingProgramEntry({ 2, 2, "Ground Dribbling" });
	_configurationAppService->changeTrainingProgramEntryDuration({ 2, 0, std::chrono::seconds(7) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 2, 1, std::chrono::seconds(7) });
	_configurationAppService->changeTrainingProgramEntryDuration({ 2, 2, std::chrono::seconds(7) });


	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}