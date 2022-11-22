#include "pch.h"
#include "TrainingProgramDisplayMinimal.h"

namespace training
{
	void TrainingProgramDisplayMinimal::renderOneFrame(const std::shared_ptr<GameWrapper>& gameWrapper, CanvasWrapper canvas, const TrainingProgramExecutionData& data)
	{
		TrainingProgramDisplay::renderOneFrame(gameWrapper, canvas, data);
		auto renderInfo = getRenderInfo(gameWrapper);
		drawInfo(canvas, renderInfo);
	}

	RenderInfo TrainingProgramDisplayMinimal::getRenderInfo(const std::shared_ptr<GameWrapper>& gameWrapper) const
	{
		auto renderInfo = RenderInfo{};
		renderInfo.LeftBorder = 0.0f;
		renderInfo.RightBorder = gameWrapper->GetScreenSize().X;
		renderInfo.BottomBorder = gameWrapper->GetScreenSize().Y;
		renderInfo.TopBorder = (int)((float)renderInfo.BottomBorder * 0.98f);

		renderInfo.Width = renderInfo.RightBorder - renderInfo.LeftBorder;
		renderInfo.Height = renderInfo.BottomBorder - renderInfo.TopBorder;

		renderInfo.TextWidthFactor = (float)gameWrapper->GetScreenSize().X / 1920.0f / 2.0f;
		renderInfo.TextHeightFactor = (float)gameWrapper->GetScreenSize().Y / 1080.0f / 2.0f;
		return renderInfo;
	}

	void TrainingProgramDisplayMinimal::drawInfo(CanvasWrapper& canvas, const RenderInfo& renderInfo) const
	{
		// Background
		// default color
		float bgAlpha = 255.0f;
		LinearColor bgColor{ 0.0f, 0.0f, 0.0f, bgAlpha };
		LinearColor textColor{ 255.0f, 255.0f, 255.0f, 255.0f };

		// transition
		if (_data.TrainingStepStartTime.has_value())
		{
			auto millisecondsInCurrentTrainingStep = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _data.TrainingStepStartTime.value());
			if (millisecondsInCurrentTrainingStep.count() < 2000)
			{
				float bgFlash = fmin(1.0f, (2000.0f - (float)millisecondsInCurrentTrainingStep.count()) / 2000.0f) * 255.0f;
				bgColor = { bgFlash, bgFlash, bgFlash, bgAlpha };
				textColor = { 255.0f - bgFlash, 255.0f - bgFlash, 255.0f - bgFlash, 255.0f };
			}
		}

		// finished
		std::string text = "";
		if (_data.TrainingFinishedTime.has_value())
		{
			auto millisecondsSinceFinished = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _data.TrainingFinishedTime.value()).count();
			if (millisecondsSinceFinished < 5000) {
				bgColor = { 0.0f, 255.0f, 0.0f, bgAlpha };
				textColor = { 0.0f, 0.0f, 0.0f, 255.0f };
				text = "Finished!";
			}
		}

		if(_data.NumberOfSteps > 0 && _data.TrainingStepStartTime.has_value()) {
			// Training Step Number and Name
			const auto shortenedName = _data.TrainingStepName.substr(0, 40);
			std::string timeLeft = "-";
			if(!_data.CurrentStepIsUntimed) {
				// Remaining Step Time
				const auto remainingStepTime = std::chrono::milliseconds(_data.TimeLeftInCurrentTrainingStep);
				const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingStepTime);
				const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingStepTime - minutes);
				timeLeft = fmt::format("{}m {}s", minutes.count(), seconds.count());
			}
			text = fmt::format("{} / {}    {}    {}", _data.TrainingStepNumber + 1, _data.NumberOfSteps, shortenedName, timeLeft);
		}
		
		if(text.length() == 0) {
			return;
		}

		Vector2F stringSize = canvas.GetStringSize(text);
		float centerX = renderInfo.LeftBorder + (renderInfo.Width / 2.0f);

		// bakground
		Vector2 min { centerX - (stringSize.X / 2.0f), renderInfo.TopBorder };
		Vector2 max { centerX + (stringSize.X / 2.0f), renderInfo.BottomBorder };
		min.X -= 50.0f;
		max.X += 50.0f;
		canvas.SetColor(bgColor);
		canvas.DrawRect(min, max);

		// text
		canvas.SetPosition(Vector2{ (int)round(centerX - (stringSize.X / 2.0f)), renderInfo.TopBorder + (int)floor(renderInfo.Height * 0.2f) });
		canvas.SetColor(textColor);
		canvas.DrawString(
			text,
			2.0f * renderInfo.TextWidthFactor,
			2.0f * renderInfo.TextHeightFactor
		);
	}
	
}