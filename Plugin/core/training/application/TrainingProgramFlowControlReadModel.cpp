#include <pch.h>
//#include "TrainingProgramFlowControlReadModel.h"
//
//namespace Core::Training::Application
//{
//	void TrainingProgramFlowControlReadModel::on(const std::shared_ptr<Configuration::Events::TrainingProgramListChangedEvent>& changeEvent)
//	{
//		MostRecentChangeEvent = changeEvent;
//	}
//	void TrainingProgramFlowControlReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent)
//	{
//		MostRecentSelectionEvent = selectionEvent;
//	}
//	void TrainingProgramFlowControlReadModel::on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>&)
//	{
//		MostRecentSelectionEvent = nullptr;
//	}
//	void TrainingProgramFlowControlReadModel::on(const std::shared_ptr<Events::TrainingProgramStateChangedEvent>& stateEvent)
//	{
//		MostRecentStateEvent = stateEvent;
//	}
//	void TrainingProgramFlowControlReadModel::dispatchEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent)
//	{
//		if (auto listChangeEvent = std::dynamic_pointer_cast<Configuration::Events::TrainingProgramListChangedEvent>(genericEvent);
//			listChangeEvent != nullptr)
//		{
//			on(listChangeEvent);
//		}
//		else if (auto selectionEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectedEvent>(genericEvent);
//				 selectionEvent != nullptr)
//		{
//			on(selectionEvent);
//		}
//		else if (auto resetEvent = std::dynamic_pointer_cast<Events::TrainingProgramSelectionResetEvent>(genericEvent);
//				 resetEvent != nullptr)
//		{
//			on(resetEvent);
//		}
//		else if (auto stateEvent = std::dynamic_pointer_cast<Events::TrainingProgramStateChangedEvent>(genericEvent);
//			stateEvent != nullptr)
//		{
//			on(stateEvent);
//		}
//		// else: ignore the event
//	}
//}
