#pragma once

#include <core/configuration/events/TrainingProgramEvents.h>
#include <core/configuration/events/TrainingProgramEntryEvents.h>
#include <core/training/events/TrainingProgramFlowEvents.h>

#include <DLLImportExport.h>

#include <optional>
#include <string>
#include <chrono>
#include <vector>
#include <memory>

namespace Core::Training::Application
{
	/** Provides information required by the user in order to control the training flow. */
	class RLTT_IMPORT_EXPORT TrainingProgramFlowControlReadModel
	{
	public:

		void on(const std::shared_ptr<Configuration::Events::TrainingProgramListChangedEvent>& changeEvent);
		void on(const std::shared_ptr<Events::TrainingProgramSelectedEvent>& selectionEvent);
		void on(const std::shared_ptr<Events::TrainingProgramSelectionResetEvent>& selectionEvent);
		void on(const std::shared_ptr<Events::TrainingProgramStateChangedEvent>& stateEvent);

		/** Dispatches generic events into the discrete event handlers. */
		void dispatchEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent);

		std::shared_ptr<Configuration::Events::TrainingProgramListChangedEvent> MostRecentChangeEvent = nullptr;
		std::shared_ptr<Events::TrainingProgramSelectedEvent> MostRecentSelectionEvent = nullptr;
		std::shared_ptr<Events::TrainingProgramStateChangedEvent> MostRecentStateEvent = nullptr;
	};
}