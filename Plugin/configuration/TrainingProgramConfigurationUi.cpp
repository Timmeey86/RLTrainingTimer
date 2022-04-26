#include <pch.h>
#include "TrainingProgramConfigurationUi.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/imgui_disable.h>

namespace configuration
{
	TrainingProgramConfigurationUi::TrainingProgramConfigurationUi(
		std::shared_ptr<TrainingProgramConfigurationControl> configurationControl,
		std::function<void()> finishEditingCallback)
		: _configurationControl{ configurationControl }
		, _finishEditingCallback{ std::move(finishEditingCallback) }
	{

	}

	void TrainingProgramConfigurationUi::setCurrentTrainingProgramId(uint64_t trainingProgramId)
	{
		_trainingProgramId = trainingProgramId;
		updateCaches();
	}

	void TrainingProgramConfigurationUi::updateCaches()
	{
		_entryNameCache.clear();
		_durationCache.clear();

		auto trainingProgramData = _configurationControl->getData(_trainingProgramId);
		_programNameCache = trainingProgramData.Name;
		for (const auto& entry : trainingProgramData.Entries)
		{
			_entryNameCache.push_back(entry.Name);
			_durationCache.push_back(std::chrono::duration_cast<std::chrono::minutes>(entry.Duration).count());
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
		
		const auto entryNameCacheSize = _entryNameCache.size();
		for (auto index = (uint16_t)0; index < (uint16_t)entryNameCacheSize; index++)
		{
			trainingProgramChanged |= addProgramEntryNameTextBox(index);
			ImGui::SameLine();
			trainingProgramChanged |= addProgramEntryDurationTextBox(index);
			ImGui::SameLine();
			trainingProgramChanged |= addUpButton(index, index > 0);
			ImGui::SameLine();
			trainingProgramChanged |= addDownButton(index, index < entryNameCacheSize - 1);
			ImGui::SameLine();
			trainingProgramChanged |= addDeleteButton(index);
		}
		trainingProgramChanged |= addAddButton();

		if (trainingProgramChanged)
		{
			// Update the cache after rendering so things like swap don't get messed up, even if it would only be for a frame.
			// Lord knows what kind of crashes it could produce otherwise.
			updateCaches();
			// TODO: Trigger writing, maybe in the control classes
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
		for (const auto& duration : _durationCache)
		{
			durationSum += std::chrono::minutes(duration);
		}
		auto durationString = fmt::format("{:>3} min", durationSum.count()); // minutes max 3 digits, right aligned
		ImGui::TextUnformatted(durationString.c_str());
	}
	bool TrainingProgramConfigurationUi::addProgramEntryNameTextBox(uint16_t index)
	{
		if (ImGui::InputText(fmt::format("##entryname_{}", index).c_str(), &_entryNameCache[index]))
		{
			_configurationControl->renameEntry(_trainingProgramId, index, _entryNameCache[index]);
			return true;
		}
		return false;
	}
	bool TrainingProgramConfigurationUi::addProgramEntryDurationTextBox(uint16_t index)
	{
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
			_configurationControl->addEntry(_trainingProgramId, { "New training step", std::chrono::minutes(1) });
			return true;
		}
		return false;
	}
}