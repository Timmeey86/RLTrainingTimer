#pragma once

#include <core/configuration/domain/TrainingProgramList.h>

namespace Ui
{
	/** This class is responsible for rendering the overview of available training programs. */
	class TrainingProgramListUi
	{
	public:

		/** Initializes the training program.
		 *
		 * TODO: Use an application service as interface instead.
		 */
		void initialize(std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> trainingProgramList);

		/** Renders the current settings */
		void renderTrainingProgramList();

	private:

		// Temporary method which updates itself from the current training program list.
		// In future, this needs to be replaced by event processing
		void updateFromList();

		void addProgramNameTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry);
		void addProgramDurationLabel(const Core::Configuration::Domain::TrainingProgramListEntry& entry);
		void addUpButton(
			int lineNumber,
			const Core::Configuration::Domain::TrainingProgramListEntry& entry,
			const Core::Configuration::Domain::TrainingProgramListEntry* const previousEntry);
		void addDownButton(
			int lineNumber,
			const Core::Configuration::Domain::TrainingProgramListEntry& entry,
			const Core::Configuration::Domain::TrainingProgramListEntry* const nextEntry);
		void addEditButton(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry);
		void addDeleteButton(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry);
		void addAddButton();

		std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> _trainingProgramList;
		std::vector<Core::Configuration::Domain::TrainingProgramListEntry> _currentEntries;
		std::unordered_map<uint64_t, std::string> _entryNameCache;
	};
}