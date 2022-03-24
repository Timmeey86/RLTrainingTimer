#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "adapter/TrainingProgramListUiAdapter.h"

#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class RLTrainingTimer : public BakkesMod::Plugin::BakkesModPlugin, public Adapter::TrainingProgramListUiAdapter
{
public:
	RLTrainingTimer() = default;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

private:
};

