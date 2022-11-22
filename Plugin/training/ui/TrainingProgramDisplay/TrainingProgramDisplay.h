#pragma once

#include <bakkesmod/wrappers/canvaswrapper.h>
#include <bakkesmod/wrappers/GameWrapper.h>

#include <training/data/TrainingProgramExecutionData.h>

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
		virtual void renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas, const TrainingProgramExecutionData& data) {
			_data = data;
		}

	protected:
		virtual RenderInfo getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const = 0;

		TrainingProgramExecutionData _data;
	};
}
