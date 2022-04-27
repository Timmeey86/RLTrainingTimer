#pragma once

#include <bakkesmod/wrappers/canvaswrapper.h>
#include <bakkesmod/wrappers/gamewrapper.h>

namespace training
{
	/** Defines positions on the screen. */
	struct RenderInfo
	{
	public:
		int LeftBorder;
		int TopBorder;
		int RightBorder;
		int BottomBorder;
		int Width;
		int Height;
		float TextWidthFactor;
		float TextHeightFactor;
	};
	
	/** This class is responsible for rendering information about the current training session to the screen. */
	class TrainingProgramDisplay
	{
	public:
		/** Renders a single frame. */
		void renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas);


	private:

		void drawPanelBackground(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawRemainingProgramTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawRemainingStepTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawTrainingStepNumber(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		void drawProgramName(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
		RenderInfo getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const;
		void drawTrainingProgramStepTransition(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const;
		void drawTrainingProgramFinishedInfo(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const;
	};
}