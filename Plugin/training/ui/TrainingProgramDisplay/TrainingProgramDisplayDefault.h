#pragma once

#include "TrainingProgramDisplay.h"

namespace training
{	
	/** This class is responsible for rendering information about the current training session to the screen.
	* Default style
	*/

	class TrainingProgramDisplayDefault : public TrainingProgramDisplay
	{
	public:
		/** Renders a single frame. */
		void renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas, const TrainingProgramExecutionData& data) override;

	protected:
		RenderInfo getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const override;

	private:
		void drawPanelBackground(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawRemainingProgramTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawRemainingStepTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawTrainingStepNumber(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawProgramName(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawTrainingProgramStepTransition(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const;
		void drawTrainingProgramFinishedInfo(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const;
		void drawCenteredText(const std::string& text, CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper, float stringScale, float alpha) const;
	};
}
