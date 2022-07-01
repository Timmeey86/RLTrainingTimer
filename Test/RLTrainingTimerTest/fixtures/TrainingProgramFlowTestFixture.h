#pragma once

#include <gtest/gtest.h>
#include "../fakes/FakeTimeProvider.h"
#include "../fakes/FakeGameWrapper.h"
#include "../fakes/FakeCVarManager.h"
#include <Plugin/training/control/TrainingProgramFlowControl.h>

class TrainingProgramFlowTestFixture : public testing::Test
{
public:
	void SetUp() override
	{
		_fakeGameWrapper = std::make_shared<FakeGameWrapper>();
		_fakeTimeProvider = std::make_shared<FakeTimeProvider>();
		_fakeCVarManager = std::make_shared<FakeCVarManager>();
		sut = std::make_unique<training::TrainingProgramFlowControl>(_fakeGameWrapper, _fakeTimeProvider, _fakeCVarManager);
		sut->hookToEvents();

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

		EmptyTrainingProgram.Name = "EmptyTrainingProgram";
		EmptyTrainingProgram.Id = EmptyTrainingProgramId;

		FullyTimedTrainingProgram.Description = "Full Training Program";
		FullyTimedTrainingProgram.Duration = OneMinuteFreeplayEntry.Duration + TwoMinuteDefaultEntry.Duration + OneMinuteWorkshopEntry.Duration;
		FullyTimedTrainingProgram.Entries.push_back(OneMinuteFreeplayEntry);
		FullyTimedTrainingProgram.Entries.push_back(TwoMinuteDefaultEntry);
		FullyTimedTrainingProgram.Entries.push_back(OneMinuteWorkshopEntry);
		FullyTimedTrainingProgram.Id = FullyTimedTrainingProgramId;
		FullyTimedTrainingProgram.Name = "FullTrainingProgram";
		FullyTimedTrainingProgram.ReadOnly = false;

		MixedReadOnlyTrainingProgram.Description = "Mixed Training Program";
		MixedReadOnlyTrainingProgram.Duration = std::chrono::milliseconds(0); // a mixed timed/completion program should not have a duration
		MixedReadOnlyTrainingProgram.Entries.push_back(OneMinuteFreeplayEntry);
		MixedReadOnlyTrainingProgram.Entries.push_back(PackCompletionEntry);
		MixedReadOnlyTrainingProgram.Entries.push_back(OneMinuteWorkshopEntry);
		MixedReadOnlyTrainingProgram.Entries.push_back(TwoMinuteDefaultEntry);
		MixedReadOnlyTrainingProgram.Id = MixedTrainingProgramId;
		MixedReadOnlyTrainingProgram.Name = "MixedReadOnlyTrainingProgram";
		MixedReadOnlyTrainingProgram.ReadOnly = true;

		FullTrainingProgramList.TrainingProgramData.try_emplace(EmptyTrainingProgramId, EmptyTrainingProgram);
		FullTrainingProgramList.TrainingProgramData.try_emplace(FullyTimedTrainingProgramId, FullyTimedTrainingProgram);
		FullTrainingProgramList.TrainingProgramData.try_emplace(MixedTrainingProgramId, MixedReadOnlyTrainingProgram);
		FullTrainingProgramList.TrainingProgramOrder.push_back(EmptyTrainingProgramId);
		FullTrainingProgramList.TrainingProgramOrder.push_back(FullyTimedTrainingProgramId);
		FullTrainingProgramList.TrainingProgramOrder.push_back(MixedTrainingProgramId);
		FullTrainingProgramList.WorkshopFolderLocation = WorkshopFakeBasePath;
	};
protected:

	// Simulates an event where the game was paused
	void pauseGame()
	{
		_fakeGameWrapper->FakeIsPaused = true;
		_fakeGameWrapper->FakeEventPostMap.at(PauseEventName)("");
	}
	// Simulates an event where the game was unpaused
	void unpauseGame()
	{
		_fakeGameWrapper->FakeIsPaused = false;
		_fakeGameWrapper->FakeEventPostMap.at(PauseEventName)("");
	}
	// Simulates an event where a timer tick was sent at the given point in time
	void sendTimerTick(std::chrono::steady_clock::time_point& pointInTime)
	{
		_fakeTimeProvider->CurrentFakeTime = pointInTime;
		_fakeGameWrapper->FakeEventMap.at(TimerTickEventName)("");
	}


	std::unique_ptr<training::TrainingProgramFlowControl> sut;
	std::shared_ptr<FakeGameWrapper> _fakeGameWrapper;
	std::shared_ptr<FakeTimeProvider> _fakeTimeProvider;
	std::shared_ptr<FakeCVarManager> _fakeCVarManager;
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


	configuration::TrainingProgramData EmptyTrainingProgram;
	const std::string EmptyTrainingProgramId = "{EmptyTrainingProgram}";
	
	configuration::TrainingProgramData FullyTimedTrainingProgram;
	const std::string FullyTimedTrainingProgramId = "{FullTrainingProgram}";

	configuration::TrainingProgramData MixedReadOnlyTrainingProgram;
	const std::string MixedTrainingProgramId = "{MixedReadOnlyTrainingProgram}";

	configuration::TrainingProgramListData EmptyTrainingProgramList;
	configuration::TrainingProgramListData FullTrainingProgramList;

	const std::string WorkshopFakeBasePath = "C:\\Temp\\Workshop";

	const std::string PauseEventName = "Function Engine.WorldInfo.EventPauseChanged";
	const std::string TimerTickEventName = "Function TAGame.Replay_TA.Tick";
};