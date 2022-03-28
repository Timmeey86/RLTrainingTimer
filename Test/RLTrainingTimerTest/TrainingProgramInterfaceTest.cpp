#include <gmock/gmock.h>

#include <Plugin/core/training/domain/TrainingProgramInterface.h>
#include <Plugin/core/training/events/TrainingProgramInterfaceEvents.h>

namespace Core::Training::Test
{
	class TrainingProgramInterfaceTestFixture : public testing::Test
	{
	public:
		void SetUp() override
		{
			sut = std::make_unique<Domain::TrainingProgramInterface>();
		}
	protected:
		static const uint64_t DefaultId = 0ULL;
		static const uint16_t DefaultSteps = 5U;

		std::unique_ptr<Domain::TrainingProgramInterface> sut;
	};

	TEST_F(TrainingProgramInterfaceTestFixture, verify_fixture)
	{
		ASSERT_NE(sut, nullptr);
	}

	TEST_F(TrainingProgramInterfaceTestFixture, selectTrainingProgram_when_calledTheFirstTime_will_supplyValidIdAndInvalidPreviousId)
	{
		auto genericEvents = sut->selectTrainingProgram(DefaultId, DefaultSteps);

		ASSERT_EQ(genericEvents.size(), 1);

		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[0].get());

		ASSERT_NE(selectionEvent, nullptr);

		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, DefaultId);
		EXPECT_FALSE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());

		EXPECT_TRUE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, selectTrainingProgram_when_calledTheSecondTime_will_supplyValidIdAndPreviousId)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		auto genericEvents = sut->selectTrainingProgram(DefaultId + 1ULL, DefaultSteps);

		ASSERT_EQ(genericEvents.size(), 1);

		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[0].get());

		ASSERT_NE(selectionEvent, nullptr);

		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, DefaultId + 1ULL);
		ASSERT_TRUE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());
		EXPECT_EQ(selectionEvent->PreviouslySelectedTrainingProgramId.value(), DefaultId);

		EXPECT_TRUE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, selectTrainingProgram_when_calledWhileProgramIsRunning_will_stopPreviousProgram)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();

		auto genericEvents = sut->selectTrainingProgram(DefaultId + 1ULL, DefaultSteps);

		ASSERT_EQ(genericEvents.size(), 2);

		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvents[0].get());
		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[1].get());

		ASSERT_NE(abortEvent, nullptr);
		ASSERT_NE(selectionEvent, nullptr);

		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, DefaultId + 1ULL);
		ASSERT_TRUE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());
		EXPECT_EQ(selectionEvent->PreviouslySelectedTrainingProgramId.value(), DefaultId);

		EXPECT_TRUE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, unselectTrainingProgram_when_calledWithNoSelectedProgram_will_returnEmptyList)
	{
		auto genericEvents = sut->unselectTrainingProgram();

		EXPECT_EQ(genericEvents.size(), 0);

		EXPECT_FALSE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, unselectTrainingProgram_when_calledWithSelectedProgram_will_returnResetEvent)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);

		auto genericEvents = sut->unselectTrainingProgram();

		ASSERT_EQ(genericEvents.size(), 1);

		auto unselectionEvent = dynamic_cast<Events::TrainingProgramSelectionResetEvent*>(genericEvents.back().get());

		ASSERT_NE(unselectionEvent, nullptr);
		ASSERT_TRUE(unselectionEvent->PreviouslySelectedTrainingProgramId.has_value());
		EXPECT_EQ(unselectionEvent->PreviouslySelectedTrainingProgramId, DefaultId);

		EXPECT_FALSE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, unselectTrainingProgram_when_programIsRunning_will_returnAbortedAndUnselectedEvents)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		sut->pauseOrResumeTrainingProgram();

		auto genericEvents = sut->unselectTrainingProgram();

		ASSERT_EQ(genericEvents.size(), 2);

		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvents.at(0).get());
		auto resetEvent = dynamic_cast<Events::TrainingProgramSelectionResetEvent*>(genericEvents.at(1).get());

		ASSERT_NE(abortEvent, nullptr);
		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);
		ASSERT_NE(resetEvent, nullptr);
		EXPECT_EQ(resetEvent->PreviouslySelectedTrainingProgramId, DefaultId);

		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->trainingProgramIsSelected());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, startSelectedTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
	{
		auto genericEvent = sut->startSelectedTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);

		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, startSelectedTrainingProgram_when_programIsSelected_will_returnValidEvent)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		auto genericEvent = sut->startSelectedTrainingProgram();
		auto startEvent = dynamic_cast<Events::TrainingProgramStartedEvent*>(genericEvent.get());

		ASSERT_NE(startEvent, nullptr);

		EXPECT_EQ(startEvent->TrainingProgramId, DefaultId);

		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}

	TEST_F(TrainingProgramInterfaceTestFixture, pauseOrResumeTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
	{
		auto genericEvent = sut->pauseOrResumeTrainingProgram();
		
		EXPECT_EQ(genericEvent, nullptr);

		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, pauseOrResumeTrainingProgram_when_programIsNotRunning_will_returnNullptr)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		auto genericEvent = sut->pauseOrResumeTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);

		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
		
	}
	TEST_F(TrainingProgramInterfaceTestFixture, pauseOrResumteTrainingProgram_when_programIsRunning_will_returnValidEvent)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		auto genericEvent = sut->pauseOrResumeTrainingProgram();
		auto pauseEvent = dynamic_cast<Events::TrainingProgramPausedEvent*>(genericEvent.get());

		ASSERT_NE(pauseEvent, nullptr);

		EXPECT_EQ(pauseEvent->TrainingProgramId, DefaultId);
		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning()); // Running should still be true while paused
		EXPECT_TRUE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, pauseOrResumeTrainingProgram_when_programIsPaused_will_returnValidEvent)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		sut->pauseOrResumeTrainingProgram();
		auto genericEvent = sut->pauseOrResumeTrainingProgram();
		auto resumeEvent = dynamic_cast<Events::TrainingProgramResumedEvent*>(genericEvent.get());

		ASSERT_NE(resumeEvent, nullptr);

		EXPECT_EQ(resumeEvent->TrainingProgramId, DefaultId);
		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning()); 
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, finishRunningTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
	{
		auto genericEvent = sut->finishRunningTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, finishRunningTrainingProgram_when_programIsNotRunning_will_returnNullptr)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		auto genericEvent = sut->finishRunningTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning()); // not because it stopped but because it wasn't running in the first place
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, finishRunningTrainingProgram_when_programIsRunning_will_returnValidEvent)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		auto genericEvent = sut->finishRunningTrainingProgram();
		auto stopEvent = dynamic_cast<Events::TrainingProgramFinishedEvent*>(genericEvent.get());

		ASSERT_NE(stopEvent, nullptr);

		EXPECT_EQ(stopEvent->TrainingProgramId, DefaultId);
		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	// finishRunningTrainingProgram shall only be called when the program reaches the end of its lifetime, which induces that it cannot be paused.
	// If the user interface contains a "Stop" button which allows stopping the program at any given point in time, abortRunningTrainingProgram() shall be called instead.
	TEST_F(TrainingProgramInterfaceTestFixture, finishRunningTrainingProgram_when_programIsPaused_will_returnNullptr)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		sut->pauseOrResumeTrainingProgram();
		auto genericEvent = sut->finishRunningTrainingProgram();

		ASSERT_EQ(genericEvent, nullptr);

		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning()); // The training program should be unaffected by the stop call
		EXPECT_TRUE(sut->runningTrainingProgramIsPaused()); // The training program is still paused
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, abortRunningTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
	{
		auto genericEvent = sut->abortRunningTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, abortRunningTrainingProgram_when_programIsNotRunning_will_returnNullptr)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);

		auto genericEvent = sut->abortRunningTrainingProgram();

		EXPECT_EQ(genericEvent, nullptr);
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, abortRunningTrainingProgram_when_programIsRunning_will_abort)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();

		auto genericEvent = sut->abortRunningTrainingProgram();
		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvent.get());

		EXPECT_NE(abortEvent, nullptr);
		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);

		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
	TEST_F(TrainingProgramInterfaceTestFixture, abortRunningTrainingProgram_when_programIsPaused_will_unpauseAndAbort)
	{
		sut->selectTrainingProgram(DefaultId, DefaultSteps);
		sut->startSelectedTrainingProgram();
		sut->pauseOrResumeTrainingProgram();

		auto genericEvent = sut->abortRunningTrainingProgram();
		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvent.get());

		EXPECT_NE(abortEvent, nullptr);
		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);

		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
	}
}
