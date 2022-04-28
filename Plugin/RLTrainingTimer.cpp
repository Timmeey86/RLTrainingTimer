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
	flowControl->hookToEvents(gameWrapper);

	// Create a plugin window for starting, stopping etc programs. This internally also creates an overlay which is displayed while training is being executed
	initTrainingProgramFlowControlUi(gameWrapper, flowControl);
	
	
	/* INITIALIZATION */

	// Restore any previously stored training program
	// TODO

	// TEMP DEBUG
	auto id = trainingProgramListControl->addTrainingProgram();
	singleTrainingProgramControl->addEntry(id, { "First Step", std::chrono::milliseconds(10000) });
	singleTrainingProgramControl->addEntry(id, { "Second Step", std::chrono::milliseconds(10000) });
	singleTrainingProgramControl->addEntry(id, { "Last Step", std::chrono::milliseconds(10000) });

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}