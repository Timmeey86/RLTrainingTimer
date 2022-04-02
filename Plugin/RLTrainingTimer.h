#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include <adapter/TrainingProgramListUiAdapter.h>
#include <adapter/RocketLeagueEventAdapter.h>

#include "ui/TrainingProgramFlowControlUi.h"

#include <core/training/application/TrainingApplicationService.h>

#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class RLTrainingTimer : public BakkesMod::Plugin::BakkesModPlugin, public Adapter::TrainingProgramListUiAdapter, public Ui::TrainingProgramFlowControlUi
{
public:
	RLTrainingTimer() = default;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

private:

	std::shared_ptr<Core::Training::Application::TrainingApplicationService> _trainingAppService;
	std::shared_ptr<Adapter::RocketLeagueEventAdapter> _eventAdapter;
};

