#include <pch.h>
#include "TrainingProgramConfigurationUi.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/imgui_disable.h>

#include <unordered_set>

namespace configuration
{
	TrainingProgramConfigurationUi::TrainingProgramConfigurationUi(
		std::shared_ptr<TrainingProgramConfigurationControl> configurationControl,
		std::function<void()> finishEditingCallback)
		: _configurationControl{ configurationControl }
		, _finishEditingCallback{ std::move(finishEditingCallback) }
	{

	}

	void TrainingProgramConfigurationUi::setCurrentTrainingProgramId(const std::string& trainingProgramId)
	{
		_trainingProgramId = trainingProgramId;
		updateCaches();
	}

	void TrainingProgramConfigurationUi::updateCaches()
	{
		_entryNameCache.clear();
		_trainingPackCodeCache.clear();
		_workshopMapPathCache.clear();
		_durationCache.clear();
		_selectedTypeCache.clear();
		_selectedCompletionModeCache.clear();

		auto trainingProgramData = _configurationControl->getData(_trainingProgramId);
		_programNameCache = trainingProgramData.Name;
		for (const auto& entry : trainingProgramData.Entries)
		{
			_entryNameCache.push_back(entry.Name);
			_trainingPackCodeCache.push_back(entry.TrainingPackCode);
			_workshopMapPathCache.push_back(entry.WorkshopMapPath);
			_durationCache.push_back(std::chrono::duration_cast<std::chrono::minutes>(entry.Duration).count());
			_selectedTypeCache.push_back((int)entry.Type);
			_selectedCompletionModeCache.push_back((int)entry.TimeMode);
		}
	}

	void TrainingProgramConfigurationUi::renderTrainingProgram()
	{
		bool trainingProgramChanged = false;

		addBackButton();
		ImGui::Separator();
		ImGui::TextUnformatted("Training Summary");
		trainingProgramChanged |= addProgramNameTextBox();
		ImGui::SameLine();
		addProgramDurationLabel();
		ImGui::Separator();
		ImGui::TextUnformatted("Training steps");
		trainingProgramChanged |= addAddButton();
		ImGui::Separator();
		
		const auto entryNameCacheSize = _entryNameCache.size();
		for (auto index = (uint16_t)0; index < (uint16_t)entryNameCacheSize; index++)
		{
			ImGui::TextUnformatted(fmt::format("Step #{}", index + 1).c_str());
			// New line!
			trainingProgramChanged |= addProgramEntryNameTextBox(index);
			// New line!
			trainingProgramChanged |= addCompletionModeDropdown(index);
			trainingProgramChanged |= addProgramEntryDurationTextBox(index);
			// New line!
			trainingProgramChanged |= addTypeDropdown(index);
			// New line!
			// New line (only visible when custom training is selected)
			trainingProgramChanged |= addCustomTrainingCodeTextBox(index);
			// only visible when workshop map is selected
			trainingProgramChanged |= addWorkshopMapPathTextBox(index);
			// New line!
			trainingProgramChanged |= addUpButton(index, index > 0);
			ImGui::SameLine();
			trainingProgramChanged |= addDownButton(index, index < entryNameCacheSize - 1);
			ImGui::SameLine();
			trainingProgramChanged |= addDeleteButton(index);
			ImGui::Separator();
		}

		if (trainingProgramChanged)
		{
			// Update the cache after rendering so things like swap don't get messed up, even if it would only be for a frame.
			// Lord knows what kind of crashes it could produce otherwise.
			updateCaches();
		}
	}


	void TrainingProgramConfigurationUi::addBackButton()
	{
		if (ImGui::Button("##entryback", "Back"))
		{
			_finishEditingCallback();
		}
	}
	bool TrainingProgramConfigurationUi::addProgramNameTextBox()
	{
		// Note: ## hides the label
		if (ImGui::InputText("##name", &_programNameCache))
		{
			_configurationControl->renameProgram(_trainingProgramId, _programNameCache);
			return true;
		}
		return false;
	}
	void TrainingProgramConfigurationUi::addProgramDurationLabel()
	{
		std::chrono::minutes durationSum = {};
		for (auto index = 0; index < _durationCache.size(); index++ )
		{
			if (_selectedCompletionModeCache[index] == 0) // timed
			{
				durationSum += std::chrono::minutes(_durationCache[index]);
			}
		}
		auto durationString = fmt::format("{:>3} min", durationSum.count()); // minutes max 3 digits, right aligned
		ImGui::TextUnformatted(durationString.c_str());
	}
	bool TrainingProgramConfigurationUi::addProgramEntryNameTextBox(uint16_t index)
	{
		ImGui::TextUnformatted("Name");
		ImGui::SameLine();
		ImGui::PushItemWidth(600.0f);
		if (ImGui::InputText(fmt::format("##entryname_{}", index).c_str(), &_entryNameCache[index]))
		{
			_configurationControl->renameEntry(_trainingProgramId, index, _entryNameCache[index]);
			return true;
		}
		ImGui::PopItemWidth();
		return false;
	}
	bool TrainingProgramConfigurationUi::addProgramEntryDurationTextBox(uint16_t index)
	{
		if (_selectedCompletionModeCache[index] != 0)
		{
			return false; // Duration is only available in the "timed" mode
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);
		auto textChanged = ImGui::InputInt(fmt::format("##entryduration_{}", index).c_str(), &_durationCache[index]);
		if (textChanged)
		{
			if (_durationCache[index] < 0)
			{
				_durationCache[index] = 0;
			}
			else
			{
				const auto durationInMinutes = std::chrono::minutes(_durationCache[index]);
				const auto durationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationInMinutes);
				_configurationControl->changeEntryDuration(_trainingProgramId, index, durationInMilliseconds);
			}
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::TextUnformatted("min");
		return textChanged;
	}
	bool TrainingProgramConfigurationUi::addUpButton(uint16_t index, bool hasPreviousEntry)
	{
		ImGui::Disable disable(!hasPreviousEntry);
		if (ImGui::ArrowButton(fmt::format("##entryup_{}", index).c_str(), ImGuiDir_Up) && hasPreviousEntry)
		{
			_configurationControl->swapEntries(_trainingProgramId, index, index - 1);
			return true;
		}
		return false;
	}
	bool TrainingProgramConfigurationUi::addDownButton(uint16_t index, bool hasNextEntry)
	{
		ImGui::Disable disable(!hasNextEntry);
		if (ImGui::ArrowButton(fmt::format("##entrydown_{}", index).c_str(), ImGuiDir_Down) && hasNextEntry)
		{
			_configurationControl->swapEntries(_trainingProgramId, index, index + 1);
			return true;
		}
		return false;
	}
	bool TrainingProgramConfigurationUi::addDeleteButton(uint16_t index)
	{
		if (ImGui::Button(fmt::format("##entrydelete_{}", index).c_str(), "Delete"))
		{
			_configurationControl->removeEntry(_trainingProgramId, index);
			return true;
		}
		return false;
	}
	bool TrainingProgramConfigurationUi::addAddButton()
	{
		if (ImGui::Button("##entryadd", "Add"))
		{
			_configurationControl->addEntry(_trainingProgramId, 
				{ 
					"New training step", 
					TrainingProgramEntryType::Unspecified,
					"", // no code
					"", // no workshop map path
					TrainingProgramCompletionMode::Timed,
					std::chrono::minutes(1),
					"", // no notes
					VarianceSettings() // default
				});
			return true;
		}
		return false;
	}

	bool TrainingProgramConfigurationUi::addCompletionModeDropdown(uint16_t index)
	{
		static std::vector<const char*> selectedNames;
		if (selectedNames.size() <= index)
		{
			selectedNames.push_back("");
		}

		std::unordered_map<int, std::string> enumNames;
		enumNames.emplace(0, "Timed");
		enumNames.emplace(1, "Pack Completion");

		ImGui::TextUnformatted("Completion Mode");
		ImGui::SameLine();
		ImGui::PushItemWidth(150.0f);
		auto changed = false;
		selectedNames[index] = enumNames[_selectedCompletionModeCache[index]].c_str();
		if (ImGui::BeginCombo(fmt::format("##completionmode_{}", index).c_str(), selectedNames[index]))
		{
			for (const auto& [intValue, stringValue] : enumNames)
			{
				const auto isSelected = intValue == _selectedCompletionModeCache[index];
				if (ImGui::Selectable(stringValue.c_str(), isSelected))
				{
					_configurationControl->changeEntryCompletionMode(_trainingProgramId, index, (TrainingProgramCompletionMode)intValue);

					// TODO: Move this logic to configurationControl
					if (intValue == 1 && _selectedTypeCache[index] != 2)
					{
						// "Completion Mode" only makes sense in custom training
						_configurationControl->changeEntryType(_trainingProgramId, index, TrainingProgramEntryType::CustomTraining);
					}
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		return changed;
	}

	bool TrainingProgramConfigurationUi::addTypeDropdown(uint16_t index)
	{
		static std::vector<const char*> selectedNames;
		if (selectedNames.size() <= index)
		{
			selectedNames.push_back("");
		}

		std::unordered_map<int, std::string> enumNames;
		enumNames.emplace(0, "Default");
		enumNames.emplace(1, "Free Play");
		enumNames.emplace(2, "Custom Training");
		enumNames.emplace(3, "Workshop Map");

		ImGui::TextUnformatted("Type ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150.0f);
		auto changed = false;
		selectedNames[index] = enumNames[_selectedTypeCache[index]].c_str();
		if (ImGui::BeginCombo(fmt::format("##trainingtype_{}", index).c_str(), selectedNames[index]))
		{
			for (const auto& [intValue, stringValue] : enumNames)
			{
				const auto isSelected = intValue == _selectedTypeCache[index];
				if (ImGui::Selectable(stringValue.c_str(), isSelected))
				{
					_configurationControl->changeEntryType(_trainingProgramId, index, (TrainingProgramEntryType)intValue);
					changed = true;

					if (intValue != 2 && _selectedCompletionModeCache[index] != 0)
					{
						// If anything else than custom training is selected, the only supported completion mode is "Timed"
						_configurationControl->changeEntryCompletionMode(_trainingProgramId, index, TrainingProgramCompletionMode::Timed);
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		return changed;
	}

	bool TrainingProgramConfigurationUi::addCustomTrainingCodeTextBox(uint16_t index)
	{
		auto changed = false;

		// Display the text box for custom training pack code only when custom training is selected
		if (_selectedTypeCache[index] == (int)TrainingProgramEntryType::CustomTraining)
		{
			ImGui::TextUnformatted(fmt::format("Code ", index).c_str());
			ImGui::SameLine();
			ImGui::PushItemWidth(300.0f);
			if (ImGui::InputText(fmt::format("##trainingpackcode_{}", index).c_str(), &_trainingPackCodeCache[index]))
			{
				_configurationControl->changeTrainingPackCode(_trainingProgramId, index, _trainingPackCodeCache[index]);
				changed = true;
			}
			ImGui::PopItemWidth();
		}
		return changed;
	}

	bool TrainingProgramConfigurationUi::addWorkshopMapPathTextBox(uint16_t index)
	{
		auto changed = false;

		// Display the text box only when workshop is selected
		if (_selectedTypeCache[index] == (int)TrainingProgramEntryType::WorkshopMap)
		{
			ImGui::TextUnformatted("!!! WARNING !!!");
			ImGui::TextUnformatted("Loading workshop maps will sometimes crash the game. The reason is unknown and this plugin can't fix that.");
			ImGui::TextUnformatted("Enter the relative file path to the .upk file, based on the folder configured on the main configuration page");
			ImGui::TextUnformatted(fmt::format("Path ", index).c_str());
			ImGui::SameLine();
			if (ImGui::InputText(fmt::format("##workshopath_{}", index).c_str(), &_workshopMapPathCache[index]))
			{
				_configurationControl->changeWorkshopMapPath(_trainingProgramId, index, _workshopMapPathCache[index]);
				changed = true;
			}
			ImGui::PopItemWidth();
		}
		return changed;
	}
}