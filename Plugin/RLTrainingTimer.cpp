#include <pch.h>
#include "RLTrainingTimer.h"

#include <training/control/TrainingProgramFlowControl.h>
#include <configuration/control/TrainingProgramRepository.h>

BAKKESMOD_PLUGIN(RLTrainingTimer, "RL Training Timer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLTrainingTimer::onLoad()
{
	_globalCvarManager = cvarManager;

	/* CONFIGURATION PART */

	// Allow storing & restoring of training programs
	auto trainingProgramRepository = std::make_shared<configuration::TrainingProgramRepository>(gameWrapper);

	// Allow manipulating the list of training programs
	auto trainingProgramDataMap = std::make_shared<std::map<std::string, configuration::TrainingProgramData>>();
	auto trainingProgramListControl = std::make_shared<configuration::TrainingProgramListConfigurationControl>(trainingProgramDataMap, trainingProgramRepository);

	// Allow manipulating a single training program, and notify the training program list when a program changes
	auto notificationFunc = [trainingProgramListControl]() { trainingProgramListControl->notifyReceivers(); };
	auto singleTrainingProgramControl = std::make_shared<configuration::TrainingProgramConfigurationControl>(trainingProgramDataMap, notificationFunc);

	// Initialize the plugin settings UI with those configuration objects
	initConfigurationUi(gameWrapper, singleTrainingProgramControl, trainingProgramListControl);

	/* TRAINING EXECUTION PART */

	// Create a handler for the execution of training programs
	auto flowControl = std::make_shared<training::TrainingProgramFlowControl>(gameWrapper);
	trainingProgramListControl->registerTrainingProgramListReceiver(flowControl); // Updates the flow control whenever any training program changes, gets added, gets deleted etc
	flowControl->hookToEvents(gameWrapper);

	// Create a plugin window for starting, stopping etc programs. This internally also creates an overlay which is displayed while training is being executed
	initTrainingProgramFlowControlUi(gameWrapper, flowControl);

	// Restore any previously stored training program
	trainingProgramListControl->restoreData();

	// Allow injection training programs from other plugins
	_trainingProgramInjector = std::make_shared<injection::TrainingProgramInjector>(
		cvarManager,
		trainingProgramListControl,
		flowControl
		);
	_trainingProgramInjector->registerNotifiers();

	cvarManager->log("Loaded RLTrainingTimer plugin");
}

void RLTrainingTimer::onUnload()
{
	cvarManager->log("Unloaded RLTrainingTimer plugin");
}