#pragma once

#include <core/configuration/application/TrainingProgramConfigurationService.h>
#include <core/configuration/events/TrainingProgramEvents.h>

#include <ui/TrainingProgramListUi.h>
#include <ui/TrainingProgramUi.h>

#include <bakkesmod/plugin/PluginSettingsWindow.h>

#include <IMGUI/imgui.h>

namespace Adapter
{
	/** This class is an adapter between the bakkesmod settings page (i.e. the user interface)
	  * and the training program list aggregate of the domain model.
	  * 
	  * We are currently using a lazy approach where this is a bidirectional adapter, i.e. it forwards commands to the application service,
	  * and it acts as an event receiver, too.
	  * 
	  * An important concept to understand for bakkesmod settings pages is that RenderSettings() will be called
	  * on each frame. Since we only want to update the UI in case of domain events (rather than polling each frame),
	  * this class makes use of a local cache which lives as long as the settings page is open.
	  * Another thing to keep in mind is that the plugin loading mechanism of bakkesmod dictates that the
	  * RLTrainingTimer plugin class must inherit this adapter, therefore we may not have a custom constructor.
	  */
	class TrainingProgramListUiAdapter 
		: public BakkesMod::Plugin::PluginSettingsWindow
		, public Core::Configuration::Application::IConfigurationEventReceiver
	{
	public:
		TrainingProgramListUiAdapter();

		void connectToAppService(std::shared_ptr<Core::Configuration::Application::TrainingProgramConfigurationService> appService);

		// Inherited via PluginSettingsWindow
		void RenderSettings() override;
		std::string GetPluginName() override;
		void SetImGuiContext(uintptr_t ctx) override;

		// Inherited via IConfigurationEventReceiver
		void processEvent(const std::shared_ptr<Core::Kernel::DomainEvent>& genericEvent) override;

	private: 

		std::shared_ptr<Ui::TrainingProgramUi> _trainingProgramUi;
		std::shared_ptr<Ui::TrainingProgramListUi> _trainingProgramListUi;
		bool _isEditing = false;
		
		std::shared_ptr<Core::Configuration::Application::TrainingProgramConfigurationService> _appService;
	};
}