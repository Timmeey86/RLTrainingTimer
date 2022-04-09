#include "pch.h"
#include "TrainingProgramDisplayReadModel.h"

namespace Core::Training::Application
{
	void TrainingProgramDisplayReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent)
	{
		MostRecentSelectionEvent = selectionEvent;
	}
	void TrainingProgramDisplayReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>&)
	{
		MostRecentSelectionEvent = nullptr;
	}
	void TrainingProgramDisplayReadModel::on(const std::shared_ptr<Events::TrainingProgramStateChangedEvent>& stateEvent)
	{
		if (!stateEvent->StoppingIsPossible)
		{
			// This means the program is not running any longer, thus there is no current training step
			MostRecentTrainingStepEvent = nullptr;
		}
		if (stateEvent->TrainingWasFinished)
		{
			LOG("Storing finish time");
			TrainingFinishedTime = std::chrono::steady_clock::now();
		}
	}
	void TrainingProgramDisplayReadModel::on(const std::shared_ptr<Events::TrainingProgramStepChangedEvent>& changeEvent)
	{
		MostRecentTrainingStepEvent = changeEvent;
		if (changeEvent->IsValid)
		{
			TrainingStepStartTime = std::chrono::steady_clock::now();
			LOG("Resetting finish time");
			TrainingFinishedTime = {};
		}
		else
		{
			TrainingStepStartTime.reset();
		}
	}
	void TrainingProgramDisplayReadModel::on(const std::shared_ptr<Events::TrainingTimeUpdatedEvent>& stepEvent)
	{
		MostRecentTimeEvent = stepEvent;
	}

	/** Dispatches generic events into the discrete event handlers. */
	void TrainingProgramDisplayReadModel::dispatchEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent)
	{
		if (auto selectionEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectedEvent>(genericEvent);
			selectionEvent != nullptr)
		{
			on(selectionEvent);
		}
		else if (auto resetEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectionResetEvent>(genericEvent);
			resetEvent != nullptr)
		{
			on(resetEvent);
		}
		else if (auto stateEvent = std::dynamic_pointer_cast<Events::TrainingProgramStateChangedEvent>(genericEvent);
			stateEvent != nullptr)
		{
			on(stateEvent);
		}
		else if (auto stepEvent = std::dynamic_pointer_cast<Events::TrainingProgramStepChangedEvent>(genericEvent);
			stepEvent != nullptr)
		{
			on(stepEvent);
		}
		else if (auto timeEvent = std::dynamic_pointer_cast<Events::TrainingTimeUpdatedEvent>(genericEvent);
			timeEvent != nullptr)
		{
			on(timeEvent);
		}
	}
}
