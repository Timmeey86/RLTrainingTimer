#pragma once

#include <IMGUI/imgui.h>
#include <core/configuration/domain/TrainingProgramList.h>

#include "bakkesmod/plugin/PluginSettingsWindow.h"

namespace Adapter
{
	/** This class is an adapter between the bakkesmod settings page (i.e. the user interface)
	  * and the training program list aggregate of the domain model.
	  * 
	  * This class will most likely adapt to an appliation service instead at some point.
	  * 
	  * An important concept to understand for bakkesmod settings pages is that RenderSettings() will be called
	  * on each frame. Since we only want to update the UI in case of domain events (rather than polling each frame),
	  * this class makes use of a local cache which lives as long as the settings page is open.
	  * Another thing to keep in mind is that the plugin loading mechanism of bakkesmod dictates that the
	  * RLTrainingTimer plugin class must inherit this adapter, therefore we may not have a custom constructor.
	  */
	class TrainingProgramListUiAdapter : public BakkesMod::Plugin::PluginSettingsWindow
	{
	public:
		// Inherited via PluginSettingsWindow
		virtual void RenderSettings() override;
		virtual std::string GetPluginName() override;
		virtual void SetImGuiContext(uintptr_t ctx) override;

		void subscribe(std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> trainingProgramList);
		void unsubscribe();

	private:
		// Temporary method which updates itself from the current training program list.
		// In future, this needs to be replaced by event processing
		void updateFromList();


		std::vector<Core::Configuration::Domain::TrainingProgramListEntry> _currentEntries;
		std::unordered_map<uint64_t, std::string> _entryNameCache;

		std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> _trainingProgramList;
	};
}