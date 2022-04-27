#pragma once

#include "TrainingProgramDisplay.h"

#include "../control/TrainingProgramFlowControl.h"
#include "TrainingProgramDisplay.h"

#include <bakkesmod/plugin/pluginwindow.h>
#include <bakkesmod/wrappers/gamewrapper.h>

#include <memory>

namespace training
{
	/** This class is responsible for rendering the control elements for a training program.
	 *
	 * Note that the main plugin class will have to inherit this class (that's the way plugin windows work in Bakkesmod)
	 * This also means this class may only have a default constructor.
	 */
	class TrainingProgramFlowControlUi : public BakkesMod::Plugin::PluginWindow
	{
	public:
		void initTrainingProgramFlowControlUi(
			std::shared_ptr<GameWrapper> gameWrapper, 
			std::shared_ptr<TrainingProgramFlowControl> flowControl
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
		
	private:
		void renderOneFrame();
		void toggleMenu();

		bool _shouldBlockInput = false;
		bool _isWindowOpen = false;

		std::vector<std::string> _exceptionMessages;
		std::shared_ptr<TrainingProgramFlowControl> _flowControl = nullptr;
		std::shared_ptr<TrainingProgramDisplay> _trainingProgramDisplay = std::make_shared<TrainingProgramDisplay>();
	};
}