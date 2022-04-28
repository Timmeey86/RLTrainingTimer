#pragma once

#include "TrainingProgramListConfigurationUi.h"
#include "TrainingProgramConfigurationUi.h"
#include "../control/TrainingProgramConfigurationControl.h"
#include "../control/TrainingProgramListConfigurationControl.h"

#include <bakkesmod/plugin/PluginSettingsWindow.h>

#include <IMGUI/imgui.h>

namespace configuration
{
	/** This class alternates between rendering the list of training programs or a single training program as needed. */
	class ConfigurationUi
		: public BakkesMod::Plugin::PluginSettingsWindow
	{
	public:
		void initConfigurationUi(
			std::shared_ptr<GameWrapper> gameWrapper, 
			std::shared_ptr<TrainingProgramConfigurationControl> singleProgramControl, 
			std::shared_ptr<TrainingProgramListConfigurationControl> programListControl);

		// Inherited via PluginSettingsWindow
		void RenderSettings() override;
		std::string GetPluginName() override;
		void SetImGuiContext(uintptr_t ctx) override;

	private: 

		bool _isEditing = false;

		std::shared_ptr<TrainingProgramConfigurationUi> _singleTrainingProgramUi;
		std::shared_ptr<TrainingProgramListConfigurationUi> _trainingProgramOverviewUi;
	};
}