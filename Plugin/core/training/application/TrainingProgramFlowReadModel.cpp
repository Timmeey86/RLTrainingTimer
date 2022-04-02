#include "pch.h"
#include "TrainingProgramFlowReadModel.h"

namespace Core::Training::Application
{
	void TrainingProgramFlowReadModel::updateTrainingProgramListEntries(std::vector<Configuration::Domain::TrainingProgramListEntry> trainingProgramEntries)
	{
		_trainingProgramEntries = trainingProgramEntries;
		reset();
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>&)
	{
		reset();
	}
	void TrainingProgramFlowReadModel::reset()
	{
		_selectedTrainingProgramId.reset();
		_selectedTrainingProgramIndex.reset();
		_startingIsPossible = false;
		_pausingIsPossible = false;
		_resumingIsPossible = false;
		_stoppingIsPossible = false;
		_currentTrainingStepName.clear();
		_currentTrainingStepDuration = 0;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent)
	{
		_selectedTrainingProgramId = selectionEvent->SelectedTrainingProgramId;
		auto trainingProgramIterator = std::find_if(_trainingProgramEntries.cbegin(), _trainingProgramEntries.cend(), [this](const Configuration::Domain::TrainingProgramListEntry& entry) {
			return entry.TrainingProgramId == _selectedTrainingProgramId.value();
		});
		if (trainingProgramIterator != _trainingProgramEntries.end())
		{
			_selectedTrainingProgramIndex = (uint16_t)std::distance(_trainingProgramEntries.cbegin(), trainingProgramIterator);
			_startingIsPossible = true;
		}
		else
		{
			LOG("WARNING: Could not locate a trainnig program with ID {} in the list of training programs.", _selectedTrainingProgramId.value());
			_selectedTrainingProgramIndex = 0;
			_startingIsPossible = false;
		}
		_pausingIsPossible = false;
		_resumingIsPossible = false;
		_stoppingIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramStartedEvent>&)
	{
		_startingIsPossible = false;
		_pausingIsPossible = true;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramPausedEvent>&)
	{
		_pausingIsPossible = false;
		_resumingIsPossible = true;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramResumedEvent>&)
	{
		_resumingIsPossible = false;
		_pausingIsPossible = true;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramFinishedEvent>&)
	{
		_startingIsPossible = true;
		_pausingIsPossible = false;
		_resumingIsPossible = false;
		_stoppingIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramAbortedEvent>&)
	{
		_startingIsPossible = true;
		_pausingIsPossible = false;
		_resumingIsPossible = false;
		_stoppingIsPossible = false;
	}
	void TrainingProgramFlowReadModel::on(const std::shared_ptr<Events::TrainingProgramStepActivatedEvent>& stepEvent)
	{
		_currentTrainingStepName = stepEvent->TrainingProgramStepName;
		_currentTrainingStepDuration = stepEvent->TrainingProgramStepDuration;
		_currentTrainingStepNumber = stepEvent->TrainingProgramStepNumber;
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
