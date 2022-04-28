#include "pch.h"
#include "TrainingProgramDisplay.h"

namespace training
{
	void TrainingProgramDisplay::renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas, const TrainingProgramExecutionData& data)
	{
		_data = data;

		if (data.TrainingFinishedTime.has_value())
		{
			drawTrainingProgramFinishedInfo(canvas, getRenderInfo(gameWrapper), gameWrapper);
		}

		if(data.NumberOfSteps == 0)
		{
			// A program with zero steps can't be run, so we can't display anything really helpful
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

	void TrainingProgramDisplay::drawPanelBackground(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Background
		canvas.SetColor(LinearColor{ 0.0f, 0.0f, 255.0f, 150.0f });
		canvas.DrawRect(Vector2{ renderInfo.LeftBorder, renderInfo.TopBorder }, Vector2{ renderInfo.RightBorder, renderInfo.BottomBorder });
	}
	void TrainingProgramDisplay::drawProgramName(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Program Name
		canvas.SetColor(LinearColor{ 255.0f, 255.0f, 255.0f, 255.0f });
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.005f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.2f) });
		auto shortenedName = fmt::format("{}{}", _data.TrainingIsPaused ? "PAUSED - " : "", _data.TrainingProgramName);
		shortenedName = shortenedName.substr(0, 40);
		canvas.DrawString(shortenedName, 1.6f * renderInfo.TextWidthFactor, 1.6f * renderInfo.TextHeightFactor);
	}
	void TrainingProgramDisplay::drawTrainingStepNumber(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Training Step Number and Name
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.25f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.1f) });
		const auto shortenedName = _data.TrainingStepName.substr(0, 40);
		canvas.DrawString(
			fmt::format("{} / {}: {}", _data.TrainingStepNumber + 1, _data.NumberOfSteps, shortenedName),
			2.5f * renderInfo.TextWidthFactor,
			2.5f * renderInfo.TextHeightFactor
		);
	}
	void TrainingProgramDisplay::drawRemainingStepTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Remaining Step Time
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.7f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.1f) });
		const auto remainingStepTime = std::chrono::milliseconds(_data.TimeLeftInCurrentTrainingStep);
		const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingStepTime);
		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingStepTime - minutes);
		canvas.DrawString(
			fmt::format("Next: {}m {}s", minutes.count(), seconds.count()),
			2.5f * renderInfo.TextWidthFactor,
			2.5f * renderInfo.TextHeightFactor
		);
	}
	void TrainingProgramDisplay::drawRemainingProgramTime(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Remaining Program Time
		canvas.SetPosition(Vector2{ renderInfo.LeftBorder + (int)floor(renderInfo.Width * 0.9f), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.2f) });
		const auto remainingProgramTime = std::chrono::milliseconds(_data.TimeLeftInProgram);
		const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingProgramTime);
		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingProgramTime - minutes);
		canvas.DrawString(
			fmt::format("End: {}m {}s", minutes.count(), seconds.count()),
			2.0f * renderInfo.TextWidthFactor,
			2.0f * renderInfo.TextHeightFactor
		);
	}

	void drawCenteredText(const std::string& text, CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper, float stringScale, float alpha)
	{
		auto screenSize = gameWrapper->GetScreenSize();
		Vector2F textSize = {};
		// Try to fit the text in the screen
		stringScale += 1.0f;
		do
		{
			stringScale -= .25f;
			textSize = canvas.GetStringSize(text, stringScale * renderInfo.TextWidthFactor, stringScale * renderInfo.TextHeightFactor);
		} while (textSize.X > (float)screenSize.X && stringScale >= .0f);

		// Calculate the text position so it is centered
		auto middleX = (float)screenSize.X / 2.0f;
		auto middleY = (float)screenSize.Y / 2.0f;
		auto textLeft = middleX - textSize.X / 2.0f;
		auto textTop = middleY - textSize.Y / 2.0f;

		// Draw the string
		canvas.SetColor(LinearColor{ 255.0f, 255.0f, 0.0f, alpha });
		canvas.SetPosition(Vector2{ (int)floor(textLeft), (int)floor(textTop) });
		canvas.DrawString(text, stringScale * renderInfo.TextWidthFactor, stringScale * renderInfo.TextHeightFactor, true);
	}

	void TrainingProgramDisplay::drawTrainingProgramStepTransition(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const
	{
		if (_data.TrainingStepStartTime.has_value())
		{
			auto millisecondsInCurrentTrainingStep = (_data.DurationOfCurrentTrainingStep - _data.TimeLeftInCurrentTrainingStep).count();
			if (millisecondsInCurrentTrainingStep < 5000)
			{
				auto trainingStepName = _data.TrainingStepName;
				// alpha: 100% for two seconds, then fade out
				auto textAlpha = fmin(1.0f, (5000.0f - (float)millisecondsInCurrentTrainingStep) / 3000.0f) * 255.0f;
				auto stringScale = 10.0f;

				drawCenteredText(trainingStepName, canvas, renderInfo, gameWrapper, stringScale, textAlpha);
			}
			if (auto msLeftInStep = _data.TimeLeftInCurrentTrainingStep.count();
				msLeftInStep <= 3000)
			{
				auto secondsLeft = (int)ceil((float)msLeftInStep / 1000.0f);
				if (secondsLeft > 0)
				{
					auto textAlpha = (float)(msLeftInStep - (secondsLeft - 1LL) * 1000LL) * 0.155f + 100.0f;
					auto stringScale = 20.0f;
					drawCenteredText(std::to_string(secondsLeft), canvas, renderInfo, gameWrapper, stringScale, textAlpha);
				}
			}
		}
	}

	void TrainingProgramDisplay::drawTrainingProgramFinishedInfo(CanvasWrapper& canvas, const RenderInfo& renderInfo, const std::shared_ptr<GameWrapper>& gameWrapper) const
	{
		auto millisecondsSinceFinished = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _data.TrainingFinishedTime.value()).count();
		if (millisecondsSinceFinished < 5000)
		{
			auto stringScale = 20.0f;
			auto textAlpha = 255.0f - (float)millisecondsSinceFinished / 5000.0f * 255.0f;
			auto text = "FINISHED";

			drawCenteredText(text, canvas, renderInfo, gameWrapper, stringScale, textAlpha);
		}
	}
}
