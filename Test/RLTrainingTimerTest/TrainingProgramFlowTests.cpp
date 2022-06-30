#include <gmock/gmock.h>
#include "mocks/IGameWrapperMock.h"

#include <Plugin/training/control/TrainingProgramFlowControl.h>

namespace Core::Training::Test
{
	class TrainingProgramFlowTestFixture : public testing::Test
	{
	public:
		void SetUp() override
		{
			_gameWrapperMock = std::make_shared<::testing::StrictMock<IGameWrapperMock>>();
			sut = std::make_unique<training::TrainingProgramFlowControl>(_gameWrapperMock);
		};
	protected:
		std::unique_ptr<training::TrainingProgramFlowControl> sut;
		std::shared_ptr<IGameWrapperMock> _gameWrapperMock;
	};

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
}

