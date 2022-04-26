#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include <adapter/RocketLeagueEventAdapter.h>

#include <configuration/ConfigurationUi.h>

#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class RLTrainingTimer : public BakkesMod::Plugin::BakkesModPlugin, public configuration::ConfigurationUi//, public Ui::TrainingProgramFlowControlUi
{
public:
	RLTrainingTimer() = default;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

private:
};

