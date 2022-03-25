#pragma once

#include <core/configuration/domain/TrainingProgram.h>
#include <memory>

namespace Ui
{
	/** This class is responsible for rendering the details of a single training as well as editing that program. */
	class TrainingProgramUi
	{
	public:

		/** Constructor. Asks for a function to be called when editing has finished. */
		TrainingProgramUi(std::function<void()> finishEditingFunc);

		/** Initializes the UI for a training program.
		 *
		 * This should be called whenever a new training program shall be edited.
		 */
		void setCurrentTrainingProgram(std::shared_ptr<Core::Configuration::Domain::TrainingProgram> trainingProgram);

		/** Renders the current settings. */
		void renderTrainingProgram();

	private:

		void updateCaches();

		void addBackButton();
		void addProgramNameTextBox();
		void addProgramDurationLabel();
		void addProgramEntryNameTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramEntry& entry);
		void addProgramEntryDurationTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramEntry& entry);
		void addUpButton(int lineNumber, bool hasPreviousEntry);
		void addDownButton(int lineNumber, bool hasNextEntry);
		void addDeleteButton(int lineNumber);
		void addAddButton();

		std::shared_ptr<Core::Configuration::Domain::TrainingProgram> _trainingProgram = nullptr;
		std::string _programNameCache;
		std::vector<std::string> _entryNameCache;
		std::vector<int> _durationCache;
		std::function<void()> _finishEditingFunc;
	};
}