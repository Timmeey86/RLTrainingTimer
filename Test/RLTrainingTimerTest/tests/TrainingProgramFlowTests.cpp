#include "../fixtures/TrainingProgramFlowTestFixture.h"

namespace test
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
		EXPECT_FALSE(executionData.ProgramHasUntimedSteps);

		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_FALSE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.SkippingIsPossible);
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

		resumeGame();

		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, executionData_when_trainingIsPaused_will_forwardPausedState)
	{



		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);

		sut->pauseTrainingProgram();

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);

		sut->resumeTrainingProgram();

		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, flowData_when_trainingIsPaused_will_updatePauseButtons)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto oldFlowData = sut->getCurrentFlowData();
		EXPECT_TRUE(oldFlowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(oldFlowData.SwitchingIsPossible);
		EXPECT_FALSE(oldFlowData.StartingIsPossible);
		EXPECT_TRUE(oldFlowData.PausingIsPossible);
		EXPECT_FALSE(oldFlowData.ResumingIsPossible);
		EXPECT_TRUE(oldFlowData.StoppingIsPossible);
		EXPECT_TRUE(oldFlowData.SkippingIsPossible);
		EXPECT_FALSE(sut->getCurrentExecutionData().TrainingIsPaused);

		sut->pauseTrainingProgram();

		auto flowData = sut->getCurrentFlowData();
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_TRUE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SkippingIsPossible);

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, flowData_when_trainingIsResumed_will_updatePauseButtons)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto oldFlowData = sut->getCurrentFlowData();

		sut->pauseTrainingProgram();
		sut->resumeTrainingProgram();

		auto flowData = sut->getCurrentFlowData();
		EXPECT_EQ(flowData.PausingIsPossible, oldFlowData.PausingIsPossible);
		EXPECT_EQ(flowData.ResumingIsPossible, oldFlowData.ResumingIsPossible);
		EXPECT_EQ(flowData.SelectedTrainingProgramIndex.has_value(), oldFlowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_EQ(flowData.StartingIsPossible, oldFlowData.StartingIsPossible);
		EXPECT_EQ(flowData.StoppingIsPossible, oldFlowData.StoppingIsPossible);
		EXPECT_EQ(flowData.SwitchingIsPossible, oldFlowData.SwitchingIsPossible);
		EXPECT_EQ(flowData.SkippingIsPossible, oldFlowData.SkippingIsPossible);
	}
	
	TEST_F(TrainingProgramFlowTestFixture, startingTrainingProgram_when_gameIsPaused_will_startInPausedState)
	{
		pauseGame();

		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();
		sut->pauseTrainingProgram();
		sut->resumeTrainingProgram();

		auto flowData = sut->getCurrentFlowData();
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_TRUE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SkippingIsPossible);

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, pausingAndResumingGame_when_trainingProgramIsPaused_will_notStartInPausedState)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();
		sut->pauseTrainingProgram();

		pauseGame();
		resumeGame();

		auto flowData = sut->getCurrentFlowData();
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_FALSE(flowData.PausingIsPossible);
		EXPECT_TRUE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SkippingIsPossible);

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);
	}

	TEST_F(TrainingProgramFlowTestFixture, pausingAndResumingTrainingProgram_when_gameIsPaused_will_notLeavePausedState)
	{
		pauseGame();

		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		sut->pauseTrainingProgram();
		sut->resumeTrainingProgram();

		auto flowData = sut->getCurrentFlowData();
		EXPECT_TRUE(flowData.SelectedTrainingProgramIndex.has_value());
		EXPECT_TRUE(flowData.SwitchingIsPossible);
		EXPECT_FALSE(flowData.StartingIsPossible);
		EXPECT_TRUE(flowData.PausingIsPossible);
		EXPECT_FALSE(flowData.ResumingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
		EXPECT_TRUE(flowData.SkippingIsPossible);

		EXPECT_TRUE(sut->getCurrentExecutionData().TrainingIsPaused);
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
		EXPECT_FALSE(flowData.SkippingIsPossible);

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
		EXPECT_FALSE(flowData.SkippingIsPossible);

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
		EXPECT_EQ(flowDataAfterStarting.SkippingIsPossible, flowDataBeforeStarting.SkippingIsPossible);
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
		EXPECT_TRUE(flowData.SkippingIsPossible);
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
		EXPECT_FALSE(executionData.ProgramHasUntimedSteps);
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
		EXPECT_FALSE(flowData.SkippingIsPossible);
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

	TEST_F(TrainingProgramFlowTestFixture, handleTimerTick_when_someTimeHasPassed_will_updateTimeValuesProperly)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		// Advance to the second step
		sendTimerTick(_fakeTimeProvider->CurrentFakeTime + OneMinuteFreeplayEntry.Duration);
		// Advance within the second step
		auto secondStepStartTime = _fakeTimeProvider->CurrentFakeTime;
		auto secondStepDelta = std::chrono::milliseconds(3456);
		sendTimerTick(_fakeTimeProvider->CurrentFakeTime + secondStepDelta);

		auto executionData = sut->getCurrentExecutionData();

		EXPECT_EQ(executionData.TimeLeftInCurrentTrainingStep.count(), (TwoMinuteDefaultEntry.Duration - secondStepDelta).count());
		EXPECT_EQ(executionData.TimeLeftInProgram.count(), (TwoMinuteDefaultEntry.Duration + OneMinuteWorkshopEntry.Duration - secondStepDelta).count());
		EXPECT_EQ(executionData.TrainingStepStartTime.value().time_since_epoch().count(), secondStepStartTime.time_since_epoch().count());
	}

	TEST_F(TrainingProgramFlowTestFixture, skipTrainingProgramStep_when_programIsNotActive_will_doNothing)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);

		auto executionDataBefore = sut->getCurrentExecutionData();

		sut->activateNextTrainingProgramStep();

		auto executionDataAfter = sut->getCurrentExecutionData();

		EXPECT_EQ(executionDataBefore.TrainingStepNumber, executionDataAfter.TrainingStepNumber);
	}

	TEST_F(TrainingProgramFlowTestFixture, skipTrainingProgramStep_when_programIsPaused_will_loadNextStep)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();
		sut->pauseTrainingProgram();

		auto executionDataBefore = sut->getCurrentExecutionData();

		sut->activateNextTrainingProgramStep();

		auto executionDataAfter = sut->getCurrentExecutionData();

		EXPECT_EQ(executionDataAfter.TrainingStepNumber, executionDataBefore.TrainingStepNumber + 1);
		// New training step needs to be properly initialized
		EXPECT_EQ(executionDataAfter.TimeLeftInCurrentTrainingStep.count(), executionDataAfter.DurationOfCurrentTrainingStep.count());
		// Time left in program must be reduced by what was left from previous training step
		EXPECT_EQ(executionDataAfter.TimeLeftInProgram.count(), (executionDataBefore.TimeLeftInProgram - executionDataBefore.TimeLeftInCurrentTrainingStep).count());
	}

	TEST_F(TrainingProgramFlowTestFixture, skipTrainingProgramStep_when_programIsRunning_will_loadNextStep)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto executionDataBefore = sut->getCurrentExecutionData();

		sut->activateNextTrainingProgramStep();

		auto executionDataAfter = sut->getCurrentExecutionData();

		EXPECT_EQ(executionDataAfter.TrainingStepNumber, executionDataBefore.TrainingStepNumber + 1);
		// New training step needs to be properly initialized
		EXPECT_EQ(executionDataAfter.TimeLeftInCurrentTrainingStep.count(), executionDataAfter.DurationOfCurrentTrainingStep.count());
		// Time left in program must be reduced by what was left from previous training step
		EXPECT_EQ(executionDataAfter.TimeLeftInProgram.count(), (executionDataBefore.TimeLeftInProgram - executionDataBefore.TimeLeftInCurrentTrainingStep).count());
	}

	TEST_F(TrainingProgramFlowTestFixture, skipTrainingProgramStep_when_lastStepIsActive_will_endProgram)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto executionDataBefore = sut->getCurrentExecutionData();

		sut->activateNextTrainingProgramStep();

		auto executionDataAfter = sut->getCurrentExecutionData();

		EXPECT_EQ(executionDataBefore.TrainingStepNumber + 1, executionDataAfter.TrainingStepNumber);
	}
}