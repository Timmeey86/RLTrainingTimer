#pragma once

#include <core/configuration/events/TrainingProgramEvents.h>

#include <memory>
#include <functional>

namespace Ui
{
	/** This class is responsible for rendering the overview of available training programs as well as editing this list. */
	class TrainingProgramListUi
	{
	public:

		/** Constructor. Expectes a function to be called when switching to editing of a training program. */
		TrainingProgramListUi(
			std::function<void(uint64_t)> startEditingFunc,
			std::function<void()> addTrainingProgramFunc,
			std::function<void(uint64_t)> removeTrainingProgramFunc,
			std::function<void(uint64_t, const std::string&)> renameTrainingProgramFunc,
			std::function<void(uint64_t, uint64_t)> swapTrainingProgramsFunc
		);

		/** Renders the current settings */
		void renderTrainingProgramList();

		/** Updates the internal cache using the parameters of the given event. */
		void adaptToEvent(const std::shared_ptr<Core::Configuration::Events::TrainingProgramListChangedEvent>& changeEvent);

	private:

		void addProgramNameTextBox(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info);
		void addProgramDurationLabel(const Core::Configuration::Events::TrainingProgramInfo& info);
		void addUpButton(
			uint16_t index,
			const Core::Configuration::Events::TrainingProgramInfo& info,
			const Core::Configuration::Events::TrainingProgramInfo* const previousInfo);
		void addDownButton(
			uint16_t index,
			const Core::Configuration::Events::TrainingProgramInfo& info,
			const Core::Configuration::Events::TrainingProgramInfo* const nextInfo);
		void addEditButton(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info);
		void addDeleteButton(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info);
		void addAddButton();

		// Caches requried for editing in the UI
		std::unordered_map<uint64_t, std::string> _entryNameCache;

		// Caches for the current values to be rendered
		std::shared_ptr<Core::Configuration::Events::TrainingProgramListChangedEvent> _mostRecentChangeEvent = nullptr;

		// Functions for forwarding events
		std::function<void(uint64_t)> _startEditingFunc;
		std::function<void()> _addTrainingProgramFunc;
		std::function<void(uint64_t)> _removeTrainingProgramFunc;
		std::function<void(uint64_t, const std::string&)> _renameTrainingProgramFunc;
		std::function<void(uint64_t, uint64_t)> _swapTrainingProgramsFunc;
	};
}