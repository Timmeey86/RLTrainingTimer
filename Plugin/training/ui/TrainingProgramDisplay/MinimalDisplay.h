#pragma once

#include "TrainingProgramDisplay.h"

namespace training
{	
	/** This class is responsible for rendering information about the current training session to the screen.
	* Minimal style
	*/

	class MinimalDisplay : public TrainingProgramDisplay
	{
	public:
		/** Renders a single frame. */
		void renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas, const TrainingProgramExecutionData& data) override;

	protected:
		RenderInfo getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const override;
	
	private:
		void drawInfo(CanvasWrapper& canvas, const RenderInfo& renderInfo) const;
	};
}
