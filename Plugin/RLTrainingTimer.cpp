#include <pch.h>
#include "RLTrainingTimer.h"

#include <training/control/TrainingProgramFlowControl.h>

BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;

	/* CONFIGURATION PART */

	// Allow manipulating the list of training programs
	auto trainingProgramDataMap = std::make_shared<std::map<uint64_t, configuration::TrainingProgramData>>();
	auto trainingProgramListControl = std::make_shared<configuration::TrainingProgramListConfigurationControl>(trainingProgramDataMap);

	// Allow manipulating a single training program, and notify the training program list when a program changes
	auto notificationFunc = [trainingProgramListControl]() { trainingProgramListControl->notifyReceivers(); };
	auto singleTrainingProgramControl = std::make_shared<configuration::TrainingProgramConfigurationControl>(trainingProgramDataMap, notificationFunc);

	// Initialize the plugin settings UI with those configuration objects
	initConfigurationUi(gameWrapper, singleTrainingProgramControl, trainingProgramListControl);

	// Allow storing & restoring of training programs
	// TODO

	/* TRAINING EXECUTION PART */

	// Create a handler for the execution of training programs
	auto flowControl = std::make_shared<training::TrainingProgramFlowControl>();
	trainingProgramListControl->registerTrainingProgramListReceiver(flowControl); // Updates the flow control whenever any training program changes, gets added, gets deleted etc

	// Create a plugin window for starting, stopping etc programs
	// TODO

	// Create a display which will be visible while training is active
	// TODO

	
	/* INITIALIZATION */

	// Restore any previously stored training program
	// TODO

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}