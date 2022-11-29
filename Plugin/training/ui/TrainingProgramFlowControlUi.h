#pragma once

#include "../control/TrainingProgramFlowControl.h"
#include "TrainingProgramDisplay/TrainingProgramDisplayDefault.h"
#include "TrainingProgramDisplay/TrainingProgramDisplayMinimal.h"
#include "IErrorDisplay.h"

#include <bakkesmod/plugin/pluginwindow.h>
#include <bakkesmod/wrappers/gamewrapper.h>

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
			std::shared_ptr<CVarManagerWrapper> cvarManager
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
			Default,
			Minimal,
			None
		};
		BarStyle _barStyle = BarStyle::Default;
		bool addBarStyleDropdown();

		std::vector<std::string> _exceptionMessages;
		std::shared_ptr<CVarManagerWrapper> _cvarManager;
		std::shared_ptr<TrainingProgramFlowControl> _flowControl = nullptr;
		std::shared_ptr<TrainingProgramDisplay> _trainingProgramDisplayDefault = std::make_shared<TrainingProgramDisplayDefault>();
		std::shared_ptr<TrainingProgramDisplay> _trainingProgramDisplayMinimal = std::make_shared<TrainingProgramDisplayMinimal>();
	};
}