#include "pch.h"
#include "TrainingProgramDisplay.h"

namespace Ui
{
	void TrainingProgramDisplay::renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas)
	{
		if (_readModel.MostRecentTrainingStepEvent == nullptr || !_readModel.MostRecentTrainingStepEvent->IsValid ||
			_readModel.MostRecentSelectionEvent == nullptr || _readModel.MostRecentTimeEvent == nullptr)
		{
			// We only want to display this information in the "Running" state.
			return;
		}

		auto renderInfo = getRenderInfo(gameWrapper);

		drawPanelBackground(canvas, renderInfo);
		drawProgramName(canvas, renderInfo);
		drawTrainingStepNumber(canvas, renderInfo);
		drawRemainingStepTime(canvas, renderInfo);
		drawRemainingProgramTime(canvas, renderInfo);
		drawTrainingProgramStepTransition(canvas, renderInfo, gameWrapper);
	}

	void TrainingProgramDisplay::drawTrainingProgramStepTransition(CanvasWrapper& canvas, const Ui::RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper)
	{

		if (_readModel.TrainingStepStartTime.has_value())
		{
			auto durationSinceTrainingStepStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - _readModel.TrainingStepStartTime.value()
				).count();
			if (durationSinceTrainingStepStartTime < 5000)
			{
				auto trainingStepName = _readModel.MostRecentTrainingStepEvent->Name;
				auto stringScale = 5.0f;
				auto textSize = canvas.GetStringSize(trainingStepName, stringScale * renderInfo.TextWidthFactor, stringScale * renderInfo.TextHeightFactor);

				auto middleX = (float)gameWrapper->GetScreenSize().X / 2.0f;
				auto middleY = (float)gameWrapper->GetScreenSize().Y / 2.0f;

				// alpha: 100% for two seconds, then fade out
				auto textAlpha = fmin(1.0f, (5000.0f - (float)durationSinceTrainingStepStartTime) / 3000.0f) * 255.0f;

				auto textLeft = middleX - textSize.X / 2.0f;
				auto textTop = middleY - textSize.Y / 2.0f;

				canvas.SetColor(LinearColor{ 255.0f, 255.0f, 0.0f, textAlpha });
				canvas.SetPosition(Vector2{ (int)floor(textLeft), (int)floor(textTop) });
				canvas.DrawString(trainingStepName, stringScale * renderInfo.TextWidthFactor, stringScale * renderInfo.TextHeightFactor);
			}
		}
	}

	RenderInfo TrainingProgramDisplay::getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const
	{
		auto renderInfo = RenderInfo{};
		renderInfo.LeftBorder = 0;
		renderInfo.RightBorder = gameWrapper->GetScreenSize().X;
		renderInfo.BottomBorder = gameWrapper->GetScreenSize().Y;
		renderInfo.TopBorder = (int)((float)renderInfo.BottomBorder * 0.96f);

		renderInfo.Width = renderInfo.RightBorder - renderInfo.LeftBorder;
		renderInfo.Height = renderInfo.BottomBorder - renderInfo.TopBorder;

		renderInfo.TextWidthFactor = (float)gameWrapper->GetScreenSize().X / 1920.0f;
		renderInfo.TextHeightFactor = (float)gameWrapper->GetScreenSize().Y / 1080.0f;
		return renderInfo;
	}

	void TrainingProgramDisplay::drawPanelBackground(CanvasWrapper& canvas, const RenderInfo& renderInfo)
	{
		// Background
		canvas.SetColor(LinearColor{ 0.0f, 0.0f, 255.0f, 150.0f });
		canvas.DrawRect(Vector2{ renderInfo.LeftBorder, renderInfo.TopBorder }, Vector2{ renderInfo.RightBorder, renderInfo.BottomBorder });
	}
	void TrainingProgramDisplay::drawProgramName(CanvasWrapper& canvas, const RenderInfo& renderInfo)
	{
		// Program Name
		canvas.SetColor(LinearColor{ 255.0f, 255.0f, 255.0f, 255.0f });
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.005f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.2f) });
		const auto shortenedName = _readModel.MostRecentSelectionEvent->Name.substr(0, 40);
		canvas.DrawString(shortenedName, 1.6f * renderInfo.TextWidthFactor, 1.6f * renderInfo.TextHeightFactor);
	}
	void TrainingProgramDisplay::drawTrainingStepNumber(CanvasWrapper& canvas, const RenderInfo& renderInfo)
	{
		// Training Step Number and Name
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.25f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.1f) });
		const auto shortenedName = _readModel.MostRecentTrainingStepEvent->Name.substr(0, 50);
		canvas.DrawString(
			fmt::format("{} / {}: {}", _readModel.MostRecentTrainingStepEvent->StepNumber + 1, _readModel.MostRecentSelectionEvent->NumberOfSteps, shortenedName),
			2.5f * renderInfo.TextWidthFactor,
			2.5f * renderInfo.TextHeightFactor
		);
	}
	void TrainingProgramDisplay::drawRemainingStepTime(CanvasWrapper& canvas, const RenderInfo& renderInfo)
	{
		// Remaining Step Time
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.7f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.1f) });
		const auto remainingStepTime = std::chrono::milliseconds(_readModel.MostRecentTimeEvent->TimeLeftInCurrentTrainingStep);
		const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingStepTime);
		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingStepTime - minutes);
		canvas.DrawString(
			fmt::format("Next: {}m {}s", minutes.count(), seconds.count()),
			2.5f * renderInfo.TextWidthFactor,
			2.5f * renderInfo.TextHeightFactor
		);
	}
	void TrainingProgramDisplay::drawRemainingProgramTime(CanvasWrapper& canvas, const RenderInfo& renderInfo)
	{
		// Remaining Program Time
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.9f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.2f) });
		const auto remainingProgramTime = std::chrono::milliseconds(_readModel.MostRecentTimeEvent->TimeLeftInProgram);
		const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingProgramTime);
		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingProgramTime - minutes);
		canvas.DrawString(
			fmt::format("End: {}m {}s", minutes.count(), seconds.count()),
			2.0f * renderInfo.TextWidthFactor,
			2.0f * renderInfo.TextHeightFactor
		);
	}
}
