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
}