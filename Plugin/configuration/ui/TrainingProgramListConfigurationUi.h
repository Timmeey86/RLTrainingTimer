#pragma once

#include "../control/TrainingProgramConfigurationControl.h"
#include "../control/TrainingProgramListConfigurationControl.h"

#include <memory>
#include <functional>

namespace configuration
{
	/** This class is responsible for rendering the overview of available training programs as well as editing this list. */
	class TrainingProgramListConfigurationUi
	{
	public:

		/** Constructor. Expects a function to be called when switching to editing of a training program. */
		TrainingProgramListConfigurationUi(
			std::shared_ptr<GameWrapper> gameWrapper,
			std::shared_ptr<TrainingProgramListConfigurationControl> listConfigurationControl,
			std::shared_ptr<TrainingProgramConfigurationControl> programConfigurationControl,
			std::function<void(const std::string&)> startEditingCallback
		);

		/** Renders the current settings */
		void renderTrainingProgramList();

	private:

		void addWorkshopFolderLocationTextBos();
		void addTrainingControlWindowButton();
		void addLoadSaveButtons();

		bool addProgramNameTextBox(uint16_t index, const TrainingProgramData& info);
		void addProgramDurationLabel(const TrainingProgramData& info);
		bool addUpButton(
			uint16_t index,
			const TrainingProgramData& info,
			const TrainingProgramData* const previousInfo);
		bool addDownButton(
			uint16_t index,
			const TrainingProgramData& info,
			const TrainingProgramData* const nextInfo);
		void addEditButton(uint16_t index, const TrainingProgramData& info);
		bool addDeleteButton(uint16_t index, const TrainingProgramData& info);
		bool addSaveButton(uint16_t index, const TrainingProgramData& info);
		bool addAddButton();
		bool addLoadButton();

		// Caches requried for editing in the UI
		std::unordered_map<std::string, std::string> _entryNameCache;
		std::string _workshopFolderLocation;

		bool _firstTime = true;
		TrainingProgramListData _cache;

		std::shared_ptr<GameWrapper> _gameWrapper;

		std::shared_ptr<TrainingProgramListConfigurationControl> _listConfigurationControl;
		std::shared_ptr<TrainingProgramConfigurationControl> _programConfigurationControl;
		std::function<void(const std::string&)> _startEditingCallback;
	};
}