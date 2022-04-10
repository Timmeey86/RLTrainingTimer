#pragma once

#include "../commands/TrainingProgramConfigurationCommands.h"
#include "../domain/TrainingProgramList.h"

#include <core/kernel/DomainEvent.h>

#include <DLLImportExport.h>

#include <memory>
#include <vector>

namespace Core::Configuration::Application
{
	/* An interface for event receivers of this service's events. 
	 *
	 * Since the configuration domain needs to forward events to the training domain in addition to the configuration user interface,
	 * we use a small publish/subscribe layer instead of a read model.
	 */
	class RLTT_IMPORT_EXPORT IConfigurationEventReceiver
	{
	protected:
		IConfigurationEventReceiver() = default;
	public:
		virtual ~IConfigurationEventReceiver() = default;

		virtual void processEvent(const std::shared_ptr<Kernel::DomainEvent>& genericEvent) = 0;
		virtual void postProcessEvents() = 0;
	};

	/** An <<Application Service>> used for controlling the configuration part of training programs.
	 * Since what it does isn't really complex, this service currently contains some domain knowledge. It needs to be refactored out, should the class grow.
	 * 
	 * In a larger project, this would be an actual service.
	 * 
	 * This class will throw exceptions in case of inconsistent usage. Make sure to catch and display the exception messages when using this.
	 */ 
	class RLTT_IMPORT_EXPORT TrainingProgramConfigurationService
	{
	public:

		/** Registers a new event receiver. */
		void registerEventReceiver(IConfigurationEventReceiver* const receiver);

		void applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events);

		/** Adds a training program. */
		void addTrainingProgram(const Commands::AddTrainingProgramCommand& command);
		/** Removes a training program. */
		void removeTrainingProgram(const Commands::RemoveTrainingProgramCommand& command);
		/** Renames a training program. */
		void renameTrainingProgram(const Commands::RenameTrainingProgramCommand& command);
		/** Swaps two training programs. */
		void swapTrainingPrograms(const Commands::SwapTrainingProgramCommand& command);

		/** Adds an entry to a training program. */
		void addTrainingProgramEntry(const Commands::AddTrainingProgramEntryCommand& command);
		/** Removes entries from a training program. */
		void removeTrainingProgramEntry(const Commands::RemoveTrainingProgramEntryCommand& command);
		/** Renames a training program entry. */
		void renameTrainingProgramEntry(const Commands::ChangeTrainingProgramEntryNameCommand& command);
		/** Changes the duration of a training program entry. */
		void changeTrainingProgramEntryDuration(const Commands::ChangeTrainingProgramEntryDurationCommand& command);
		/** Swaps two entries of a training program. */
		void swapTrainingProgramEntries(const Commands::SwapTrainingProgramEntriesCommand& command);

	private:
		void publishEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events) const;

		std::vector<IConfigurationEventReceiver*> _eventReceivers;
		std::unique_ptr<Domain::TrainingProgramList> _trainingProgramList = std::make_unique<Domain::TrainingProgramList>();
		std::uint64_t _maximumId = 0ULL;
	};
}
