#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include <configuration/ui/ConfigurationUi.h>
#include <training/ui/TrainingProgramFlowControlUi.h>

#include "version.h"
#include <API/RLTrainingTimerAPI.h>
#include <DLLImportExport.h>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class RLTrainingTimer : public BakkesMod::Plugin::BakkesModPlugin, public configuration::ConfigurationUi, public training::TrainingProgramFlowControlUi
{
public:
	RLTrainingTimer() = default;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	/** Retrieves the API for adding training programs through code. */
	RLTrainingTimerAPI* getAPI() const;

private:
	RLTrainingTimerAPI* _api;
};

