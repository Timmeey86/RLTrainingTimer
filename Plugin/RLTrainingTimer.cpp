#include <pch.h>
#include "RLTrainingTimer.h"

// DUMMY
#include <core/configuration/domain/TrainingProgramList.h>

BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

void RLTrainingTimer::onLoad()
{
	// DUMMY CODE
	auto DUMMY_trainingProgramList = std::make_shared<Core::Configuration::Domain::TrainingProgramList>();
	DUMMY_trainingProgramList->addTrainingProgram(0);
	DUMMY_trainingProgramList->addTrainingProgram(1);
	DUMMY_trainingProgramList->addTrainingProgram(2);

	DUMMY_trainingProgramList->renameTrainingProgram(0, "First program");
	DUMMY_trainingProgramList->renameTrainingProgram(1, "Second program");
	DUMMY_trainingProgramList->renameTrainingProgram(2, "Third program");

	subscribe(DUMMY_trainingProgramList);

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}