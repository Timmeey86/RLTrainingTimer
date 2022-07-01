#include "../fixtures/TrainingProgramFlowTestFixture.h"

namespace Core::Training::Test
{
	TEST_F(TrainingProgramFlowTestFixture, verify_fixture)
	{
		sut->receiveListData({});

		auto executionData = sut->getCurrentExecutionData();
		auto flowData = sut->getCurrentFlowData();

		EXPECT_EQ(executionData.DurationOfCurrentTrainingStep.count(), 0);
		EXPECT_EQ(executionData.NumberOfSteps, 0);
		EXPECT_EQ(executionData.TimeLeftInCurrentTrainingStep.count(), 0);
		EXPECT_EQ(executionData.TimeLeftInProgram.count(), 0);
		EXPECT_FALSE(executionData.TrainingFinishedTime.has_value());
		EXPECT_FALSE(executionData.TrainingIsPaused);
		EXPECT_EQ(executionData.TrainingProgramName, std::string());
		EXPECT_EQ(executionData.TrainingStepNumber, 0);
		EXPECT_FALSE(executionData.TrainingStepStartTime.has_value());

		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_FALSE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_EQ(flowData.TrainingPrograms.size(), 0);
	}

	template<typename mapType, typename entryType>
	inline bool mapHasEntry(const mapType& map, const entryType& entry)
	{
		return map.find(entry) != map.end();
	}
	TEST_F(TrainingProgramFlowTestFixture, hookToEvents_when_called_will_registerEvents)
	{
		// Note: hookToEvents will be called within the test fixture
		EXPECT_TRUE(mapHasEntry(_fakeGameWrapper->FakeEventPostMap, PauseEventName));
		EXPECT_TRUE(mapHasEntry(_fakeGameWrapper->FakeEventMap, TimerTickEventName));
	}

	TEST_F(TrainingProgramFlowTestFixture, executionData_when_gameIsPaused_will_forwardPausedState)
	{
		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);

		pauseGame();

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);

		unpauseGame();

		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, selectTrainingProgram_when_programListIsEmpty_will_invalidateData)
	{
		sut->receiveListData(EmptyTrainingProgramList);
		sut->selectTrainingProgram("INVALID ID (DOES NOT EXIST)");

		auto flowData = sut->getCurrentFlowData();
		EXPECT_FALSE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_FALSE(flowData.StoppingIsPossible);

		EXPECT_EQ(sut->getCurrentExecutionData().NumberOfSteps, 0);
	}

	TEST_F(TrainingProgramFlowTestFixture, selectTrainingProgram_when_selectingFirstProgram_will_allowStarting)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(EmptyTrainingProgramId);

		auto flowData = sut->getCurrentFlowData();
		ASSERT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_EQ(flowData.SelectedTrainingProgramIndex.value(), 0); // We expect the first entry to be selected

		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_TRUE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_FALSE(flowData.StoppingIsPossible);

		EXPECT_EQ(sut->getCurrentExecutionData().NumberOfSteps, EmptyTrainingProgram.Entries.size());
	}

	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_emptyProgramIsStarted_will_returnToSameState)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(EmptyTrainingProgramId);

		auto flowDataBeforeStarting = sut->getCurrentFlowData();

		sut->startSelectedTrainingProgram();

		auto flowDataAfterStarting = sut->getCurrentFlowData();

		ASSERT_TRUE(flowDataAfterStarting.SelectedTrainingProgramIndex.has_value());
		EXPECT_EQ(flowDataAfterStarting.SelectedTrainingProgramIndex.value(), flowDataBeforeStarting.SelectedTrainingProgramIndex.value());
		EXPECT_EQ(flowDataAfterStarting.PausingIsPossible, flowDataBeforeStarting.PausingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.SwitchingIsPossible, flowDataBeforeStarting.SwitchingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.StartingIsPossible, flowDataBeforeStarting.StartingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.PausingIsPossible, flowDataBeforeStarting.PausingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.ResumingIsPossible, flowDataBeforeStarting.ResumingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.StoppingIsPossible, flowDataBeforeStarting.StoppingIsPossible);
		EXPECT_EQ(flowDataAfterStarting.PausingIsPossible, flowDataBeforeStarting.PausingIsPossible);
	}

	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_validProgramIsStarted_will_changeToRunningState)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto flowData = sut->getCurrentFlowData();

		EXPECT_TRUE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SwitchingIsPossible);
	}

	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_validProgramIsStarted_will_populateTrainingExecutionData)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto executionData = sut->getCurrentExecutionData();
		
		EXPECT_EQ(executionData.DurationOfCurrentTrainingStep, OneMinuteFreeplayEntry.Duration);
		EXPECT_EQ(executionData.NumberOfSteps, FullyTimedTrainingProgram.Entries.size());
		EXPECT_EQ(executionData.TimeLeftInCurrentTrainingStep, OneMinuteFreeplayEntry.Duration);
		EXPECT_EQ(executionData.TimeLeftInProgram, FullyTimedTrainingProgram.Duration);
		EXPECT_FALSE(executionData.TrainingFinishedTime.has_value());
		EXPECT_FALSE(executionData.TrainingIsPaused);
		EXPECT_EQ(executionData.TrainingProgramName, FullyTimedTrainingProgram.Name);
		EXPECT_EQ(executionData.TrainingStepName, OneMinuteFreeplayEntry.Name);
		EXPECT_EQ(executionData.TrainingStepNumber, 0);
		EXPECT_EQ(executionData.TrainingStepStartTime, _fakeTimeProvider->CurrentFakeTime);
	}

	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_freeplayProgramIsStarted_will_loadFreeplay)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId); // This one starts with a free play step
		sut->startSelectedTrainingProgram();

		EXPECT_TRUE(_fakeGameWrapper->ExecuteWasCalled);
		EXPECT_EQ(_fakeCVarManager->lastCommand(), "load_freeplay");
	}
	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_defaultProgramIsStarted_will_notChangeGameMode)
	{
		// Create a fake program with just a default step
		configuration::TrainingProgramData fakeProgram;
		fakeProgram.Id = "Fake";
		fakeProgram.Duration = TwoMinuteDefaultEntry.Duration;
		fakeProgram.Entries.push_back(TwoMinuteDefaultEntry);
		
		configuration::TrainingProgramListData fakeList;
		fakeList.TrainingProgramData.try_emplace(fakeProgram.Id, fakeProgram);
		fakeList.TrainingProgramOrder.push_back(fakeProgram.Id);

		sut->receiveListData(fakeList);
		sut->selectTrainingProgram(fakeProgram.Id);
		sut->startSelectedTrainingProgram();

		EXPECT_FALSE(_fakeGameWrapper->ExecuteWasCalled);
	}

	// Note: For custom training / workshop maps, we want them to always be reloaded so the unit starts at the beginning
	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_alreadyInFreePlay_will_notLoadFreePlayAgain)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId); // This one starts with a free play step

		_fakeGameWrapper->FakeIsInFreeplay = true;
		sut->startSelectedTrainingProgram();

		EXPECT_FALSE(_fakeGameWrapper->ExecuteWasCalled);
	}

	TEST_F(TrainingProgramFlowTestFixture, handleTimerTick_when_firstTrainingStepIsFinished_will_activateSecondTrainingStep)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto firstStepEndTime = _fakeTimeProvider->CurrentFakeTime + OneMinuteFreeplayEntry.Duration;

		sendTimerTick(firstStepEndTime);

		auto executionData = sut->getCurrentExecutionData();
		EXPECT_EQ(executionData.TrainingStepNumber, 1);
	}

	TEST_F(TrainingProgramFlowTestFixture, handleTimerTick_when_lastTrainingStepIsFinished_will_switchToStoppedState)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		// Note: sendTimerTick will adapt _fakeTimeProvider->CurrentFakeTime, so we can just add the duration of each step each time
		auto time = _fakeTimeProvider->CurrentFakeTime + OneMinuteFreeplayEntry.Duration;
		sendTimerTick(time);
		time += TwoMinuteDefaultEntry.Duration;
		sendTimerTick(time);
		time += OneMinuteWorkshopEntry.Duration;
		sendTimerTick(time);

		auto flowData = sut->getCurrentFlowData();

		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SwitchingIsPossible);
	}

	TEST_F(TrainingProgramFlowTestFixture, handleTimerTick_when_lastTrainingStepIsFinished_will_invalidateExecutionData)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		// Note: sendTimerTick will adapt _fakeTimeProvider->CurrentFakeTime, so we can just add the duration of each step each time
		auto time = _fakeTimeProvider->CurrentFakeTime + OneMinuteFreeplayEntry.Duration;
		sendTimerTick(time);
		time += TwoMinuteDefaultEntry.Duration;
		sendTimerTick(time);
		time += OneMinuteWorkshopEntry.Duration;
		sendTimerTick(time);

		auto executionData = sut->getCurrentExecutionData();

		ASSERT_TRUE(executionData.TrainingFinishedTime.has_value());
		EXPECT_EQ(executionData.TrainingFinishedTime.value().time_since_epoch(), _fakeTimeProvider->CurrentFakeTime.time_since_epoch());
		EXPECT_EQ(executionData.NumberOfSteps, 0); // This will only be initialized when starting the program.
	}
}