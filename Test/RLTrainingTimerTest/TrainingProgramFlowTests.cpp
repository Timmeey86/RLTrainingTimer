#include <gmock/gmock.h>

#include <Plugin/core/training/domain/TrainingProgramFlow.h>
#include <Plugin/core/training/events/TrainingProgramFlowEvents.h>

namespace Core::Training::Test
{
	class TrainingProgramFlowTestFixture : public testing::Test
	{
	public:
		void SetUp() override
		{
			sut = std::make_unique<Domain::TrainingProgramFlow>();

			// Define a training program with two steps
			ProgramWithTwoSteps.TrainingProgramId = 1;
			ProgramWithTwoSteps.TrainingProgramName = "First";
			ProgramWithTwoSteps.TrainingProgramDuration = std::chrono::milliseconds(5000);
			ProgramWithTwoSteps.TrainingProgramEntries.push_back({ "Step One", std::chrono::milliseconds(2000) });
			ProgramWithTwoSteps.TrainingProgramEntries.push_back({ "Step Two", std::chrono::milliseconds(3000) });

			// Define a training program with one step
			ProgramWithOneStep.TrainingProgramId = 42;
			ProgramWithOneStep.TrainingProgramName = "Second";
			ProgramWithOneStep.TrainingProgramDuration = std::chrono::milliseconds(12000);
			ProgramWithOneStep.TrainingProgramEntries.push_back({ "Only Step", ProgramWithOneStep.TrainingProgramDuration });

			// Define a training program with zero steps
			ProgramWithNoSteps.TrainingProgramId = 1337;
			ProgramWithNoSteps.TrainingProgramName = "Third";
			ProgramWithNoSteps.TrainingProgramDuration = std::chrono::milliseconds(0);

			// Simulate change events for these programs so the program flow knows about them
			sut->handleTrainingProgramChange(std::make_shared<Core::Configuration::Events::TrainingProgramChangedEvent>(ProgramWithTwoSteps));
			sut->handleTrainingProgramChange(std::make_shared<Core::Configuration::Events::TrainingProgramChangedEvent>(ProgramWithOneStep));
			sut->handleTrainingProgramChange(std::make_shared<Core::Configuration::Events::TrainingProgramChangedEvent>(ProgramWithNoSteps));
		}
	protected:
		Core::Configuration::Events::TrainingProgramChangedEvent ProgramWithTwoSteps;
		Core::Configuration::Events::TrainingProgramChangedEvent ProgramWithOneStep;
		Core::Configuration::Events::TrainingProgramChangedEvent ProgramWithNoSteps;

		std::unique_ptr<Domain::TrainingProgramFlow> sut;
	};

	TEST_F(TrainingProgramFlowTestFixture, verify_fixture)
	{
		ASSERT_NE(sut, nullptr);
		ASSERT_EQ(ProgramWithTwoSteps.TrainingProgramEntries.size(), 2);
		ASSERT_EQ(
			ProgramWithTwoSteps.TrainingProgramDuration.count(), 
			ProgramWithTwoSteps.TrainingProgramEntries.front().Duration.count() + ProgramWithTwoSteps.TrainingProgramEntries.back().Duration.count()
		);
		ASSERT_EQ(ProgramWithOneStep.TrainingProgramEntries.size(), 1);
		ASSERT_EQ(ProgramWithOneStep.TrainingProgramDuration.count(), ProgramWithOneStep.TrainingProgramEntries.front().Duration.count());
		ASSERT_EQ(ProgramWithNoSteps.TrainingProgramEntries.size(), 0);

		ASSERT_NE(ProgramWithTwoSteps.TrainingProgramId, ProgramWithOneStep.TrainingProgramId);
		ASSERT_NE(ProgramWithTwoSteps.TrainingProgramId, ProgramWithNoSteps.TrainingProgramId);
		ASSERT_NE(ProgramWithOneStep.TrainingProgramId, ProgramWithNoSteps.TrainingProgramId);
	}

	TEST_F(TrainingProgramFlowTestFixture, selectTrainingProgram_when_calledTheFirstTime_will_supplyValidIdAndInvalidPreviousId)
	{
		auto genericEvents = sut->selectTrainingProgram(ProgramWithTwoSteps.TrainingProgramId);

		ASSERT_EQ(genericEvents.size(), 2);

		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[0].get());
		auto stateUpdateEvent = dynamic_cast<Events::TrainingProgramStateChangedEvent*>(genericEvents[1].get());

		ASSERT_NE(selectionEvent, nullptr);
		ASSERT_NE(stateUpdateEvent, nullptr);

		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, ProgramWithTwoSteps.TrainingProgramId);
		EXPECT_EQ(selectionEvent->Name, ProgramWithTwoSteps.TrainingProgramName);
		EXPECT_EQ(selectionEvent->Duration, ProgramWithTwoSteps.TrainingProgramDuration);
		EXPECT_EQ(selectionEvent->NumberOfSteps, ProgramWithTwoSteps.TrainingProgramEntries.size());
		EXPECT_FALSE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());

		EXPECT_EQ(stateUpdateEvent->GameIsPaused, false);
		EXPECT_EQ(stateUpdateEvent->StartingIsPossible, true);
		EXPECT_EQ(stateUpdateEvent->PausingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->ResumingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->StoppingIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->SwitchingProgramIsPossible, true);

		EXPECT_EQ(sut->currentTrainingProgramState(), Core::Training::Definitions::TrainingProgramState::WaitingForStart);
	}

	TEST_F(TrainingProgramFlowTestFixture, selectTrainingProgram_when_calledTheSecondTime_will_supplyValidIdAndPreviousId)
	{
		sut->selectTrainingProgram(ProgramWithTwoSteps.TrainingProgramId);
		auto genericEvents = sut->selectTrainingProgram(ProgramWithOneStep.TrainingProgramId);

		ASSERT_EQ(genericEvents.size(), 2);

		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[0].get());
		auto stateUpdateEvent = dynamic_cast<Events::TrainingProgramStateChangedEvent*>(genericEvents[1].get());

		ASSERT_NE(selectionEvent, nullptr);
		ASSERT_NE(stateUpdateEvent, nullptr);

		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, ProgramWithOneStep.TrainingProgramId);
		EXPECT_EQ(selectionEvent->Name, ProgramWithOneStep.TrainingProgramName);
		EXPECT_EQ(selectionEvent->Duration, ProgramWithOneStep.TrainingProgramDuration);
		EXPECT_EQ(selectionEvent->NumberOfSteps, ProgramWithOneStep.TrainingProgramEntries.size());
		ASSERT_TRUE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());
		EXPECT_EQ(selectionEvent->PreviouslySelectedTrainingProgramId.value(), ProgramWithTwoSteps.TrainingProgramId);

		EXPECT_EQ(stateUpdateEvent->GameIsPaused, false);
		EXPECT_EQ(stateUpdateEvent->StartingIsPossible, true);
		EXPECT_EQ(stateUpdateEvent->PausingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->ResumingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->StoppingIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->SwitchingProgramIsPossible, true);

		EXPECT_EQ(sut->currentTrainingProgramState(), Core::Training::Definitions::TrainingProgramState::WaitingForStart);
	}

	TEST_F(TrainingProgramFlowTestFixture, selectTrainingProgram_when_calledWhileProgramIsRunning_will_stopPreviousProgram)
	{
		sut->selectTrainingProgram(ProgramWithTwoSteps.TrainingProgramId);
		sut->startSelectedTrainingProgram();

		auto genericEvents = sut->selectTrainingProgram(ProgramWithOneStep.TrainingProgramId);

		ASSERT_EQ(genericEvents.size(), 4);

		auto abortEvent = dynamic_cast<Events::TrainingProgramStepChangedEvent*>(genericEvents[0].get());
		auto abortStateEvent = dynamic_cast<Events::TrainingProgramStateChangedEvent*>(genericEvents[1].get());
		auto selectionEvent = dynamic_cast<Events::TrainingProgramSelectedEvent*>(genericEvents[2].get());
		auto stateUpdateEvent = dynamic_cast<Events::TrainingProgramStateChangedEvent*>(genericEvents[3].get());

		ASSERT_NE(abortEvent, nullptr);
		ASSERT_NE(abortStateEvent, nullptr);
		ASSERT_NE(selectionEvent, nullptr);
		ASSERT_NE(stateUpdateEvent, nullptr);

		EXPECT_FALSE(abortEvent->IsValid);

		EXPECT_EQ(abortStateEvent->GameIsPaused, false);
		EXPECT_EQ(abortStateEvent->StartingIsPossible, true);
		EXPECT_EQ(abortStateEvent->PausingProgramIsPossible, false);
		EXPECT_EQ(abortStateEvent->ResumingProgramIsPossible, false);
		EXPECT_EQ(abortStateEvent->StoppingIsPossible, false);
		EXPECT_EQ(abortStateEvent->SwitchingProgramIsPossible, true);

		EXPECT_EQ(selectionEvent->SelectedTrainingProgramId, ProgramWithOneStep.TrainingProgramId);
		EXPECT_EQ(selectionEvent->Name, ProgramWithOneStep.TrainingProgramName);
		EXPECT_EQ(selectionEvent->Duration, ProgramWithOneStep.TrainingProgramDuration);
		EXPECT_EQ(selectionEvent->NumberOfSteps, ProgramWithOneStep.TrainingProgramEntries.size());
		ASSERT_TRUE(selectionEvent->PreviouslySelectedTrainingProgramId.has_value());
		EXPECT_EQ(selectionEvent->PreviouslySelectedTrainingProgramId.value(), ProgramWithTwoSteps.TrainingProgramId);

		EXPECT_EQ(stateUpdateEvent->GameIsPaused, false);
		EXPECT_EQ(stateUpdateEvent->StartingIsPossible, true);
		EXPECT_EQ(stateUpdateEvent->PausingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->ResumingProgramIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->StoppingIsPossible, false);
		EXPECT_EQ(stateUpdateEvent->SwitchingProgramIsPossible, true);

		EXPECT_EQ(sut->currentTrainingProgramState(), Core::Training::Definitions::TrainingProgramState::WaitingForStart);
	}
//
//	TEST_F(TrainingProgramFlowTestFixture, unselectTrainingProgram_when_calledWithNoSelectedProgram_will_returnEmptyList)
//	{
//		auto genericEvents = sut->unselectTrainingProgram();
//
//		EXPECT_EQ(genericEvents.size(), 0);
//
//		EXPECT_FALSE(sut->trainingProgramIsSelected());
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//
//	TEST_F(TrainingProgramFlowTestFixture, unselectTrainingProgram_when_calledWithSelectedProgram_will_returnResetEvent)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//
//		auto genericEvents = sut->unselectTrainingProgram();
//
//		ASSERT_EQ(genericEvents.size(), 1);
//
//		auto unselectionEvent = dynamic_cast<Events::TrainingProgramSelectionResetEvent*>(genericEvents.back().get());
//
//		ASSERT_NE(unselectionEvent, nullptr);
//		ASSERT_TRUE(unselectionEvent->PreviouslySelectedTrainingProgramId.has_value());
//		EXPECT_EQ(unselectionEvent->PreviouslySelectedTrainingProgramId, DefaultId);
//
//		EXPECT_FALSE(sut->trainingProgramIsSelected());
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, unselectTrainingProgram_when_programIsRunning_will_returnAbortedAndUnselectedEvents)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		sut->startSelectedTrainingProgram();
//		sut->pauseOrResumeTrainingProgram(false, true);
//
//		auto genericEvents = sut->unselectTrainingProgram();
//
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvents.at(0).get());
//		auto resetEvent = dynamic_cast<Events::TrainingProgramSelectionResetEvent*>(genericEvents.at(1).get());
//
//		ASSERT_NE(abortEvent, nullptr);
//		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);
//		ASSERT_NE(resetEvent, nullptr);
//		EXPECT_EQ(resetEvent->PreviouslySelectedTrainingProgramId, DefaultId);
//
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->trainingProgramIsSelected());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//
//	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
//	{
//		auto genericEvent = sut->startSelectedTrainingProgram();
//
//		EXPECT_EQ(genericEvent, nullptr);
//
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//
//	TEST_F(TrainingProgramFlowTestFixture, startSelectedTrainingProgram_when_programIsSelected_will_returnValidEvent)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		auto genericEvent = sut->startSelectedTrainingProgram();
//		auto startEvent = dynamic_cast<Events::TrainingProgramStartedEvent*>(genericEvent.get());
//
//		ASSERT_NE(startEvent, nullptr);
//
//		EXPECT_EQ(startEvent->TrainingProgramId, DefaultId);
//
//		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}

	TEST_F(TrainingProgramFlowTestFixture, activateNextTrainingProgramStep_when_programIsEmpty_will_returnNoEvent)
	{
		sut->selectTrainingProgram(ProgramWithNoSteps.TrainingProgramId);
		sut->startSelectedTrainingProgram();
		auto events = sut->activateNextTrainingProgramStep();

		EXPECT_EQ(events.size(), 0);
	}

	TEST_F(TrainingProgramFlowTestFixture, pausingGame_when_programIsRunning_will_notChangeUiState)
	{

	}



//	TEST_F(TrainingProgramFlowTestFixture, finishRunningTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
//	{
//		auto genericEvent = sut->finishRunningTrainingProgram();
//
//		EXPECT_EQ(genericEvent, nullptr);
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, finishRunningTrainingProgram_when_programIsNotRunning_will_returnNullptr)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		auto genericEvent = sut->finishRunningTrainingProgram();
//
//		EXPECT_EQ(genericEvent, nullptr);
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning()); // not because it stopped but because it wasn't running in the first place
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, finishRunningTrainingProgram_when_programIsRunning_will_returnValidEvent)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		sut->startSelectedTrainingProgram();
//		auto genericEvent = sut->finishRunningTrainingProgram();
//		auto stopEvent = dynamic_cast<Events::TrainingProgramFinishedEvent*>(genericEvent.get());
//
//		ASSERT_NE(stopEvent, nullptr);
//
//		EXPECT_EQ(stopEvent->TrainingProgramId, DefaultId);
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	// finishRunningTrainingProgram shall only be called when the program reaches the end of its lifetime, which induces that it cannot be paused.
//	// If the user interface contains a "Stop" button which allows stopping the program at any given point in time, abortRunningTrainingProgram() shall be called instead.
//	TEST_F(TrainingProgramFlowTestFixture, finishRunningTrainingProgram_when_programIsPaused_will_returnNullptr)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		sut->startSelectedTrainingProgram();
//		sut->pauseOrResumeTrainingProgram(false, true);
//		auto genericEvent = sut->finishRunningTrainingProgram();
//
//		ASSERT_EQ(genericEvent, nullptr);
//
//		EXPECT_TRUE(sut->selectedTrainingProgramIsRunning()); // The training program should be unaffected by the stop call
//		EXPECT_TRUE(sut->runningTrainingProgramIsPaused()); // The training program is still paused
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, abortRunningTrainingProgram_when_noProgramIsSelected_will_returnNullptr)
//	{
//		auto genericEvent = sut->abortRunningTrainingProgram();
//
//		EXPECT_EQ(genericEvent, nullptr);
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, abortRunningTrainingProgram_when_programIsNotRunning_will_returnNullptr)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//
//		auto genericEvent = sut->abortRunningTrainingProgram();
//
//		EXPECT_EQ(genericEvent, nullptr);
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, abortRunningTrainingProgram_when_programIsRunning_will_abort)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		sut->startSelectedTrainingProgram();
//
//		auto genericEvent = sut->abortRunningTrainingProgram();
//		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvent.get());
//
//		EXPECT_NE(abortEvent, nullptr);
//		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);
//
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
//	TEST_F(TrainingProgramFlowTestFixture, abortRunningTrainingProgram_when_programIsPaused_will_unpauseAndAbort)
//	{
//		sut->selectTrainingProgram(DefaultId, DefaultSteps);
//		sut->startSelectedTrainingProgram();
//		sut->pauseOrResumeTrainingProgram(false, true);
//
//		auto genericEvent = sut->abortRunningTrainingProgram();
//		auto abortEvent = dynamic_cast<Events::TrainingProgramAbortedEvent*>(genericEvent.get());
//
//		EXPECT_NE(abortEvent, nullptr);
//		EXPECT_EQ(abortEvent->TrainingProgramId, DefaultId);
//
//		EXPECT_FALSE(sut->selectedTrainingProgramIsRunning());
//		EXPECT_FALSE(sut->runningTrainingProgramIsPaused());
//		EXPECT_FALSE(sut->currentTrainingStepNumber().has_value());
//	}
}
