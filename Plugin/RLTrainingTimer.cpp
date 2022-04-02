#include <pch.h>
#include "RLTrainingTimer.h"

// DUMMY
#include <core/configuration/domain/TrainingProgramList.h>

BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;
	// DUMMY CODE
	auto DUMMY_trainingProgramList = std::make_shared<Core::Configuration::Domain::TrainingProgramList>();
	DUMMY_trainingProgramList->addTrainingProgram(0);
	DUMMY_trainingProgramList->addTrainingProgram(1);
	DUMMY_trainingProgramList->addTrainingProgram(2);

	DUMMY_trainingProgramList->renameTrainingProgram(0, "Coaching Training Schedule");
	DUMMY_trainingProgramList->renameTrainingProgram(1, "Second program");
	DUMMY_trainingProgramList->renameTrainingProgram(2, "Third program");

	auto DUMMY_firstProgram = DUMMY_trainingProgramList->getTrainingProgram(0);
	DUMMY_firstProgram->addEntry({ "Forward Wave Dashes (No Boost)" , 2 * 60 * 1000 });
	DUMMY_firstProgram->addEntry({ "Forward Wave Dashes (With Boost)" , 3 * 60 * 1000 });
	DUMMY_firstProgram->addEntry({ "Forward Wave Dashes (Ball Cam On)" , 3 * 60 * 1000 });
	DUMMY_firstProgram->addEntry({ "Any Direction Wave Dashes" , 2 * 60 * 1000 });
	DUMMY_firstProgram->addEntry({ "Half Flips (ball cam off)" , 5 * 60 * 1000 });
	DUMMY_firstProgram->addEntry({ "Saves from the Wall (use BM)" , 5 * 60 * 1000 });


	auto DUMMY_secondProgram = DUMMY_trainingProgramList->getTrainingProgram(1);
	DUMMY_secondProgram->addEntry({ "Dummy", 180000 });

	subscribe(DUMMY_trainingProgramList);

	_trainingAppService = std::make_shared<Core::Training::Application::TrainingApplicationService>(
		DUMMY_trainingProgramList, std::make_shared<Core::Training::Domain::TrainingProgramFlow>()
		);

	_eventAdapter = std::make_shared<Adapter::RocketLeagueEventAdapter>(_trainingAppService);
	_eventAdapter->hookToEvents(gameWrapper);

	initTrainingProgramFlowControlUi(gameWrapper, _trainingAppService);


	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}