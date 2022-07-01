#pragma once

#include <gtest/gtest.h>
#include "../fakes/FakeTimeProvider.h"
#include "../fakes/FakeGameWrapper.h"
#include <Plugin/training/control/TrainingProgramFlowControl.h>

class TrainingProgramFlowTestFixture : public testing::Test
{
public:
	void SetUp() override
	{
		_fakeGameWrapper = std::make_shared<FakeGameWrapper>();
		_fakeTimeProvider = std::make_shared<FakeTimeProvider>();
		sut = std::make_unique<training::TrainingProgramFlowControl>(_fakeGameWrapper, _fakeTimeProvider);
		sut->hookToEvents(_fakeGameWrapper);

		IncompleteFreeplayEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteFreeplayEntry.Type = configuration::TrainingProgramEntryType::Freeplay;
		// No duration

		IncompleteCustomTrainingEntry.TimeMode = configuration::TrainingProgramCompletionMode::CompletePack;
		IncompleteCustomTrainingEntry.Type = configuration::TrainingProgramEntryType::CustomTraining;
		// No training pack code

		IncompleteDefaultEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteDefaultEntry.Type = configuration::TrainingProgramEntryType::Unspecified;
		// No duration

		IncompleteWorkshopEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteWorkshopEntry.Duration = IncompleteWorkshopDuration;
		IncompleteWorkshopEntry.Type = configuration::TrainingProgramEntryType::WorkshopMap;
		// No workshop map path

		OneMinuteFreeplayEntry.Type = configuration::TrainingProgramEntryType::Freeplay;
		OneMinuteFreeplayEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		OneMinuteFreeplayEntry.Duration = std::chrono::milliseconds(60 * 1000);
		OneMinuteFreeplayEntry.Name = "OneMinuteFreeplayEntry";

		PackCompletionEntry.Type = configuration::TrainingProgramEntryType::CustomTraining;
		PackCompletionEntry.TimeMode = configuration::TrainingProgramCompletionMode::CompletePack;
		PackCompletionEntry.TrainingPackCode = DummyTrainingPackCode;
		PackCompletionEntry.Name = "PackCompletionEntry";

		OneMinuteWorkshopEntry.Type = configuration::TrainingProgramEntryType::WorkshopMap;
		OneMinuteWorkshopEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		OneMinuteWorkshopEntry.WorkshopMapPath = DummyWorkshopSubPath;
		OneMinuteWorkshopEntry.Duration = std::chrono::milliseconds(60 * 1000);
		OneMinuteWorkshopEntry.Name = "OneMinuteWorkshopEntry";

		TwoMinuteDefaultEntry.Type = configuration::TrainingProgramEntryType::Unspecified;
		TwoMinuteDefaultEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		TwoMinuteDefaultEntry.Duration = std::chrono::milliseconds(2 * 60 * 1000);
		TwoMinuteDefaultEntry.Name = "TwoMinuteDefaultEntry";
	};
protected:

	void pauseGame()
	{
		_fakeGameWrapper->FakeIsPaused = true;
		_fakeGameWrapper->FakeEventPostMap.at(PauseEventName)("");
	}
	void unpauseGame()
	{
		_fakeGameWrapper->FakeIsPaused = false;
		_fakeGameWrapper->FakeEventPostMap.at(PauseEventName)("");
	}


	std::unique_ptr<training::TrainingProgramFlowControl> sut;
	std::shared_ptr<FakeGameWrapper> _fakeGameWrapper;
	std::shared_ptr<FakeTimeProvider> _fakeTimeProvider;

	configuration::TrainingProgramEntry EmptyEntry;
	configuration::TrainingProgramEntry IncompleteFreeplayEntry;
	configuration::TrainingProgramEntry IncompleteCustomTrainingEntry;
	configuration::TrainingProgramEntry IncompleteDefaultEntry;
	configuration::TrainingProgramEntry IncompleteWorkshopEntry;
	const std::chrono::milliseconds IncompleteWorkshopDuration = std::chrono::milliseconds(2000);
	configuration::TrainingProgramEntry OneMinuteFreeplayEntry;
	configuration::TrainingProgramEntry PackCompletionEntry;
	const std::string DummyTrainingPackCode = "ABC123";
	configuration::TrainingProgramEntry OneMinuteWorkshopEntry;
	const std::string DummyWorkshopSubPath = "FancyMap\\FancyMap.upk";
	configuration::TrainingProgramEntry TwoMinuteDefaultEntry;

	const std::string PauseEventName = "Function Engine.WorldInfo.EventPauseChanged";
	const std::string TimerTickEventName = "Function TAGame.Replay_TA.Tick";
};