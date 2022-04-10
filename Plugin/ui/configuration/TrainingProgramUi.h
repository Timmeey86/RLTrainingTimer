#pragma once

#include <core/configuration/events/TrainingProgramEntryEvents.h>

#include <memory>
#include <functional>

namespace Ui
{
	/** This class is responsible for rendering the details of a single training as well as editing that program. 
	 *
	 * It is coupled to lambda functions rather than objects so it is not affected by changes in the domain context,
	 * unless that change is supposed to affect the user interface (like a new setting).
	 **/
	class TrainingProgramUi
	{
	public:

		/** Constructor. Asks for a function to be called when editing has finished. */
		TrainingProgramUi(
			std::function<void()> finishEditingFunc,
			std::function<void(uint64_t, const std::string&)> renameProgramFunc,
			std::function<void(uint64_t)> addEntryFunc,
			std::function<void(uint64_t, uint16_t)> removeEntryFunc,
			std::function<void(uint64_t, uint16_t, const std::string&)> renameEntryFunc,
			std::function<void(uint64_t, uint16_t, const std::chrono::milliseconds&)> changeDurationFunc,
			std::function<void(uint64_t, uint16_t, uint16_t)> swapEntriesFunc
			);

		/** Initializes the UI for a training program.
		 *
		 * This should be called whenever a new training program shall be edited.
		 */
		void setCurrentTrainingProgramId(uint64_t trainingProgramId);

		/** Renders the current settings. */
		void renderTrainingProgram();

		/** Updates the internal cache using the parameters of the given event. */
		void adaptToEvent(const std::shared_ptr<Core::Configuration::Events::TrainingProgramChangedEvent>& changeEvent);

	private:

		void updateCaches();

		void addBackButton();
		void addProgramNameTextBox();
		void addProgramDurationLabel();
		void addProgramEntryNameTextBox(uint16_t index);
		void addProgramEntryDurationTextBox(uint16_t index);
		void addUpButton(uint16_t index, bool hasPreviousEntry);
		void addDownButton(uint16_t index, bool hasNextEntry);
		void addDeleteButton(uint16_t index);
		void addAddButton();

		uint64_t _trainingProgramId = 0;

		// Caches required for editing in the UI
		std::string _programNameCache;
		std::vector<std::string> _entryNameCache;
		std::vector<int> _durationCache; // minutes. Can't be an std::chrono type due to IMGUI

		// Caches for the current values to be rendered
		std::unordered_map<uint64_t, std::shared_ptr<Core::Configuration::Events::TrainingProgramChangedEvent>> _changeEventCache;

		// Functions for forwarding events
		std::function<void()> _finishEditingFunc;
		std::function<void(uint64_t, const std::string&)> _renameProgramFunc;
		std::function<void(uint64_t)> _addEntryFunc;
		std::function<void(uint64_t, uint16_t)> _removeEntryFunc;
		std::function<void(uint64_t, uint16_t, const std::string&)> _renameEntryFunc;
		std::function<void(uint64_t, uint16_t, const std::chrono::milliseconds&)> _changeDurationFunc;
		std::function<void(uint64_t, uint16_t, uint16_t)> _swapEntriesFunc;
	};
}