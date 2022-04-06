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
		auto leftBorder = 0;
		auto rightBorder = gameWrapper->GetScreenSize().X;
		auto bottomBorder = gameWrapper->GetScreenSize().Y;
		auto topBorder = (int)((float)bottomBorder * 0.96f);
		
		auto width = rightBorder - leftBorder;
		auto height = bottomBorder - topBorder;

		auto textWidthFactor = (float)gameWrapper->GetScreenSize().X / 1920.0f;
		auto textHeightFactor = (float)gameWrapper->GetScreenSize().Y / 1080.0f;

		// Background
		canvas.SetColor(LinearColor{ 0.0f, 0.0f, 255.0f, 100.0f });
		canvas.DrawRect(Vector2{ leftBorder, topBorder }, Vector2{ rightBorder, bottomBorder });

		// Program Name
		canvas.SetColor(LinearColor{ 255.0f, 255.0f, 255.0f, 150.0f });
		canvas.SetPosition(Vector2{ leftBorder + (int)floor(width * 0.005f), topBorder + (int)floor(height * 0.2f) });
		auto shortenedName = _readModel.MostRecentSelectionEvent->Name.substr(0, 40);
		canvas.DrawString(shortenedName, 1.6f * textWidthFactor, 1.6f * textHeightFactor);

		// Training Step Number and Name
		canvas.SetPosition(Vector2{ leftBorder + (int)floor(width * 0.25f), topBorder + (int)floor(height * 0.1f) });
		shortenedName = _readModel.MostRecentTrainingStepEvent->Name.substr(0, 50);
		canvas.DrawString(
			fmt::format("{} / {}: {}", _readModel.MostRecentTrainingStepEvent->StepNumber + 1, _readModel.MostRecentSelectionEvent->NumberOfSteps, shortenedName),
			2.5f * textWidthFactor,
			2.5f * textHeightFactor
		);

		// Remaining Step Time
		canvas.SetPosition(Vector2{ leftBorder + (int)floor(width * 0.7f), topBorder + (int)floor(height * 0.1f) });
		const auto remainingStepTime = std::chrono::milliseconds(_readModel.MostRecentTimeEvent->TimeLeftInCurrentTrainingStep);
		auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingStepTime);
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingStepTime - minutes);
		canvas.DrawString(
			fmt::format("Next: {}m {}s", minutes.count(), seconds.count()),
			2.5f * textWidthFactor, 
			2.5f * textHeightFactor
		);

		// Remaining Program Time
		canvas.SetPosition(Vector2{ leftBorder + (int)floor(width * 0.9f), topBorder + (int)floor(height * 0.2f) });
		const auto remainingProgramTime = std::chrono::milliseconds(_readModel.MostRecentTimeEvent->TimeLeftInProgram);
		minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingProgramTime);
		seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingProgramTime - minutes);
		canvas.DrawString(
			fmt::format("End: {}m {}s", minutes.count(), seconds.count()),
			2.0f * textWidthFactor,
			2.0f * textHeightFactor
		);
	}
}
