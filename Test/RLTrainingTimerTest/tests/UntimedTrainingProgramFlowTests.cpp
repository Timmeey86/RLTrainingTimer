#include "../fixtures/TrainingProgramFlowTestFixture.h"

namespace test
{
	// Just to get some separation in the test explorer
	class UntimedTrainingProgramFlowTestFixture : public TrainingProgramFlowTestFixture	{ };

	TEST_F(UntimedTrainingProgramFlowTestFixture, startUntimedTrainingProgram_when_validProgramIsStarted_will_populateTrainingExecutionData)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(UntimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		// Send one timer tick for every program entry, to make sure the program is still active and did not transition to the next steps
		for (auto index = 0; index < UntimedTrainingProgram.Entries.size(); index++)
		{
			sendTimerTick(_fakeTimeProvider->CurrentFakeTime + std::chrono::milliseconds(1));
		}

		auto executionData = sut->getCurrentExecutionData();

		EXPECT_EQ(executionData.DurationOfCurrentTrainingStep, std::chrono::milliseconds(0)); // no duration for untimed steps obviously
		EXPECT_EQ(executionData.TimeLeftInCurrentTrainingStep, std::chrono::milliseconds(0)); // should not be used in this case
		EXPECT_EQ(executionData.TimeLeftInProgram, std::chrono::milliseconds(0)); // should not be used in this case
		EXPECT_EQ(executionData.TrainingStepName, PackCompletionEntry.Name);
		EXPECT_EQ(executionData.TrainingStepNumber, 0);
	}

	TEST_F(UntimedTrainingProgramFlowTestFixture, startUntimedTrainingProgram_when_validProgramIsStarted_will_stayInFirstProgramEntry)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(UntimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		// Send one timer tick for every program entry, to make sure the program is still active and did not transition to the next steps
		for (auto index = 0; index < UntimedTrainingProgram.Entries.size(); index++)
		{
			sendTimerTick(_fakeTimeProvider->CurrentFakeTime + std::chrono::milliseconds(1));
		}

		auto flowData = sut->getCurrentFlowData();

		// Detect the "running" state
		EXPECT_TRUE(flowData.PausingIsPossible);
		EXPECT_TRUE(flowData.StoppingIsPossible);
	}

	TEST_F(UntimedTrainingProgramFlowTestFixture,startSelectedTrainingProgram_when_havingAtLeastOneUntimedEntry_will_setFlagInExecutionData)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(MixedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		EXPECT_EQ(sut->getCurrentExecutionData().ProgramHasUntimedSteps, true);
	}

	TEST_F(UntimedTrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_havingNoUntimedENtires_will_notSetFlagInExecutionData)
	{
		sut->receiveListData(FullTrainingProgramList);
		sut->selectTrainingProgram(FullyTimedTrainingProgramId);
		sut->startSelectedTrainingProgram();

		EXPECT_EQ(sut->getCurrentExecutionData().ProgramHasUntimedSteps, false);
	}
}
