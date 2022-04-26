#pragma once

#include "../control/TrainingProgramConfigurationControl.h"

#include <memory>
#include <functional>

namespace configuration
{
	/** This UI allows editing a single training program. */
	class TrainingProgramConfigurationUi
	{
	public:

		/** Constructor. Asks for a function to be called when editing has finished. */
		explicit TrainingProgramConfigurationUi(
			std::shared_ptr<TrainingProgramConfigurationControl> configurationControl,
			std::function<void()> finishEditingCallback
			);

		/** Initializes the UI for a training program.
		 *
		 * This should be called before rendering a new training program.
		 */
		void setCurrentTrainingProgramId(uint64_t trainingProgramId);

		/** Renders the current settings. */
		void renderTrainingProgram();

	private:

		void updateCaches();

		void addBackButton();
		bool addProgramNameTextBox();
		void addProgramDurationLabel();
		bool addProgramEntryNameTextBox(uint16_t index);
		bool addProgramEntryDurationTextBox(uint16_t index);
		bool addUpButton(uint16_t index, bool hasPreviousEntry);
		bool addDownButton(uint16_t index, bool hasNextEntry);
		bool addDeleteButton(uint16_t index);
		bool addAddButton();

		uint64_t _trainingProgramId = 0;

		// Caches required for editing in the UI
		std::string _programNameCache;
		std::vector<std::string> _entryNameCache;
		std::vector<int> _durationCache; // minutes. Can't be an std::chrono type due to IMGUI

		std::shared_ptr<TrainingProgramConfigurationControl> _configurationControl; 
		std::function<void()> _finishEditingCallback;
	};
}