#pragma once

#include <core/training/application/TrainingProgramDisplayReadModel.h>
#include <bakkesmod/wrappers/canvaswrapper.h>
#include <bakkesmod/wrappers/gamewrapper.h>

namespace Ui
{
	/** This class is responsible for rendering information about the current training session to the screen. */
	class TrainingProgramDisplay
	{
	public:
		/** Renders a single frame. */
		void renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas);

		/** Updates the read model for rendering. */
		inline void updateReadModel(Core::Training::Application::TrainingProgramDisplayReadModel readModel)
		{
			// TODO: Do we need to thread protect this?
			_readModel = std::move( readModel );
		}

	private:
		Core::Training::Application::TrainingProgramDisplayReadModel _readModel;
	};
}