#pragma once

#include "../control/TrainingProgramFlowControl.h"
#include "TrainingProgramDisplay/BlueBarDisplay.h"
#include "TrainingProgramDisplay/MinimalDisplay.h"
#include "IErrorDisplay.h"

#include <bakkesmod/plugin/pluginwindow.h>
#include <bakkesmod/wrappers/gamewrapper.h>
#include <external/BakkesModWiki/PersistentStorage.h>

#include <memory>

namespace training
{
	/** This class is responsible for rendering the control elements for a training program.
	 *
	 * Note that the main plugin class will have to inherit this class (that's the way plugin windows work in Bakkesmod)
	 * This also means this class may only have a default constructor.
	 * 
	 * Since having several plugin windows open at the same time isn't quite easy in bakkesmod, we abuse this window as an error display 
	 * in addition to what it usually does.
	 */
	class TrainingProgramFlowControlUi : public BakkesMod::Plugin::PluginWindow, public IErrorDisplay
	{
	public:
		void initTrainingProgramFlowControlUi(
			std::shared_ptr<GameWrapper> gameWrapper, 
			std::shared_ptr<TrainingProgramFlowControl> flowControl,
			std::shared_ptr<CVarManagerWrapper> cvarManager,
			std::shared_ptr<PersistentStorage> persistentStorage
		);

		// Inherited via PluginWindow
		virtual void Render() override;
		virtual std::string GetMenuName() override;
		virtual std::string GetMenuTitle() override;
		virtual void SetImGuiContext(uintptr_t ctx) override;
		virtual bool ShouldBlockInput() override;
		virtual bool IsActiveOverlay() override;
		virtual void OnOpen() override;
		virtual void OnClose() override;

		// Inherited via IErrorDisplay
		virtual void displayErrorMessage(const std::string& shortText, const std::string& errorDescription) override;
		virtual void clearErrorMessages() override;
		
	private:
		void renderOneFrame();
		void toggleMenu();

		bool _shouldBlockInput = false;
		bool _isWindowOpen = false;

		enum class BarStyle {
			BlueBar,
			Minimal,
			None
		};
		BarStyle _barStyle = BarStyle::Minimal;
		bool addBarStyleDropdown();

		std::vector<std::string> _exceptionMessages;
		std::shared_ptr<PersistentStorage> _persistentStorage;
		std::shared_ptr<CVarManagerWrapper> _cvarManager;
		std::shared_ptr<TrainingProgramFlowControl> _flowControl = nullptr;
		std::shared_ptr<TrainingProgramDisplay> _BlueBarDisplay = std::make_shared<BlueBarDisplay>();
		std::shared_ptr<TrainingProgramDisplay> _MinimalDisplay = std::make_shared<MinimalDisplay>();
	};
}