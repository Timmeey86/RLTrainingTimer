#include "pch.h"
#include "TrainingProgramFlowReadModel.h"

namespace Core::Training::Application
{
	void TrainingProgramFlowReadModel::updateTrainingProgramListEntries(std::vector<Configuration::Domain::TrainingProgramListEntry> trainingProgramEntries)
	{
		TrainingProgramEntries = trainingProgramEntries;
		reset();
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>&)
	{
		reset();
	}
	void TrainingProgramFlowReadModel::reset()
	{
		SelectedTrainingProgramId.reset();
		SelectedTrainingProgramIndex.reset();
		SelectedTrainingProgramName.clear();
		StartingIsPossible = false;
		PausingIsPossible = false;
		ResumingIsPossible = false;
		StoppingIsPossible = false;
		SwitchingProgramIsPossible = true;
		CurrentTrainingStepName.clear();
		CurrentTrainingStepDuration = 0;
		CurrentTrainingStepNumber = 0;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent)
	{
		SelectedTrainingProgramId = selectionEvent->SelectedTrainingProgramId;
		auto trainingProgramIterator = std::find_if(TrainingProgramEntries.cbegin(), TrainingProgramEntries.cend(), [this](const Configuration::Domain::TrainingProgramListEntry& entry) {
			return entry.TrainingProgramId == SelectedTrainingProgramId.value();
		});
		if (trainingProgramIterator != TrainingProgramEntries.end())
		{
			SelectedTrainingProgramIndex = (uint16_t)std::distance(TrainingProgramEntries.cbegin(), trainingProgramIterator);
			SelectedTrainingProgramName = TrainingProgramEntries[SelectedTrainingProgramIndex.value()].TrainingProgramName;
			StartingIsPossible = true;
		}
		else
		{
			LOG("WARNING: Could not locate a trainnig program with ID {} in the list of training programs.", SelectedTrainingProgramId.value());
			SelectedTrainingProgramIndex = 0;
			SelectedTrainingProgramName = {};
			StartingIsPossible = false;
		}
		PausingIsPossible = false;
		ResumingIsPossible = false;
		StoppingIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramStartedEvent>&)
	{
		StartingIsPossible = false;
		PausingIsPossible = true;
		StoppingIsPossible = true;
		SwitchingProgramIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramPausedEvent>& pausedEvent)
	{
		PausingIsPossible = !pausedEvent->TrainingProgramIsPaused;
		ResumingIsPossible = pausedEvent->TrainingProgramIsPaused;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramResumedEvent>& resumeEvent)
	{
		PausingIsPossible = true;
		ResumingIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramFinishedEvent>&)
	{
		StartingIsPossible = true;
		PausingIsPossible = false;
		ResumingIsPossible = false;
		StoppingIsPossible = false;
		SwitchingProgramIsPossible = true;
		CurrentTrainingStepName.clear();
		CurrentTrainingStepDuration = 0;
		CurrentTrainingStepNumber = 0;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramAbortedEvent>&)
	{
		StartingIsPossible = true;
		PausingIsPossible = false;
		ResumingIsPossible = false;
		StoppingIsPossible = false;
		SwitchingProgramIsPossible = true;
		CurrentTrainingStepName.clear();
		CurrentTrainingStepDuration = 0;
		CurrentTrainingStepNumber = 0;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramStepActivatedEvent>& stepEvent)
	{
		CurrentTrainingStepName = stepEvent->TrainingProgramStepName;
		CurrentTrainingStepDuration = stepEvent->TrainingProgramStepDuration;
		CurrentTrainingStepNumber = stepEvent->TrainingProgramStepNumber;
	}

	void TrainingProgramFlowReadModel::dispatchEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent)
	{
		if (auto resetEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectionResetEvent>(genericEvent);
			resetEvent != nullptr)
		{
			on(resetEvent);
		}
		else if (auto selectionEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectedEvent>(genericEvent);
			selectionEvent != nullptr)
		{
			on(selectionEvent);
		}
		else if (auto startEvent = std::dynamic_pointer_cast<Events::TrainingProgramStartedEvent>(genericEvent);
			startEvent != nullptr)
		{
			on(startEvent);
		}
		else if (auto pauseEvent = std::dynamic_pointer_cast<Events::TrainingProgramPausedEvent>(genericEvent);
			pauseEvent != nullptr)
		{
			on(pauseEvent);
		}
		else if (auto resumeEvent = std::dynamic_pointer_cast<Events::TrainingProgramResumedEvent>(genericEvent);
			resumeEvent != nullptr)
		{
			on(resumeEvent);
		}
		else if (auto finishEvent = std::dynamic_pointer_cast<Events::TrainingProgramFinishedEvent>(genericEvent);
			finishEvent != nullptr)
		{
			on(finishEvent);
		}
		else if (auto abortEvent = std::dynamic_pointer_cast<Events::TrainingProgramAbortedEvent>(genericEvent);
			abortEvent != nullptr)
		{
			on(abortEvent);
		}
		else if (auto stepEvent = std::dynamic_pointer_cast<Events::TrainingProgramStepActivatedEvent>(genericEvent);
			stepEvent != nullptr)
		{
			on(stepEvent);
		}
		else
		{
			// ignore
		}
	}
}
