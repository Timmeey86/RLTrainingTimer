#include <pch.h>
#include "TrainingProgramUi.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/imgui_disable.h>

namespace Ui
{
	TrainingProgramUi::TrainingProgramUi(
		std::function<void()> finishEditingFunc,
		std::function<void(uint64_t, const std::string&)> renameProgramFunc,
		std::function<void(uint64_t)> addEntryFunc,
		std::function<void(uint64_t, uint16_t)> removeEntryFunc,
		std::function<void(uint64_t, uint16_t, const std::string&)> renameEntryFunc,
		std::function<void(uint64_t, uint16_t, const std::chrono::milliseconds&)> changeDurationFunc,
		std::function<void(uint64_t, uint16_t, uint16_t)> swapEntriesFunc)
		// Note: Why do we ask for a copy and then std::move it, rather than just asking for a reference?
		// The reason is that this way the caller of this constructor knows that we are going to store the object.
		// If the caller provides an RValue to the constructor, the compiler might even elide the copy made in the constructor.
		// Therefore we have an expressive interface ("Express what you intend"), while not making unnecessary copies.
		: _finishEditingFunc{ std::move(finishEditingFunc) }
		, _renameProgramFunc{ std::move(renameProgramFunc) }
		, _addEntryFunc{ std::move(addEntryFunc) }
		, _removeEntryFunc{ std::move(removeEntryFunc) }
		, _renameEntryFunc{ std::move(renameEntryFunc) }
		, _changeDurationFunc{ std::move(changeDurationFunc) }
		, _swapEntriesFunc{ std::move(swapEntriesFunc) }
	{

	}
	void TrainingProgramUi::setCurrentTrainingProgramId(uint64_t trainingProgramId)
	{
		_trainingProgramId = trainingProgramId;
		updateCaches();
	}

	void TrainingProgramUi::adaptToEvent(const std::shared_ptr<Core::Configuration::Events::TrainingProgramChangedEvent>& changeEvent)
	{
		// Note: std::unordered_map::operator[] creates an entry if there is none, yet (which is exactly what we want here)
		_changeEventCache[changeEvent->TrainingProgramId] = changeEvent;
		if (changeEvent->TrainingProgramId == _trainingProgramId)
		{
			updateCaches();
		}
		// else: another program was updated. Its details were stored in the cache, but we don't need to update the UI now
	}

	void TrainingProgramUi::updateCaches()
	{
		_entryNameCache.clear();
		_durationCache.clear();

		auto changeEventCacheIter = _changeEventCache.find(_trainingProgramId);
		if(changeEventCacheIter != _changeEventCache.end())
		{
			auto mostRecentEvent = (*changeEventCacheIter).second;

			_programNameCache = mostRecentEvent->TrainingProgramName;
			for (const auto& entry : mostRecentEvent->TrainingProgramEntries)
			{
				_entryNameCache.push_back(entry.Name);
				_durationCache.push_back(std::chrono::duration_cast<std::chrono::minutes>(entry.Duration).count());
			}
		}
	}
	void TrainingProgramUi::renderTrainingProgram()
	{		
		if (auto changeEventCacheIter = _changeEventCache.find(_trainingProgramId); 
			changeEventCacheIter == _changeEventCache.end())
		{
			return;
		}

		addBackButton();
		ImGui::Separator();
		ImGui::TextUnformatted("Training Summary");
		addProgramNameTextBox();
		ImGui::SameLine();
		addProgramDurationLabel();
		ImGui::Separator();
		ImGui::TextUnformatted("Training steps");
		
		const auto entryNameCacheSize = _entryNameCache.size();
		for (auto index = (uint16_t)0; index < (uint16_t)entryNameCacheSize; index++)
		{
			addProgramEntryNameTextBox(index);
			ImGui::SameLine();
			addProgramEntryDurationTextBox(index);
			ImGui::SameLine();
			addUpButton(index, index > 0);
			ImGui::SameLine();
			addDownButton(index, index < entryNameCacheSize - 1);
			ImGui::SameLine();
			addDeleteButton(index);
		}
		addAddButton();
	}


	void TrainingProgramUi::addBackButton()
	{
		if (ImGui::Button("##entryback", "Back"))
		{
			_finishEditingFunc();
		}
	}
	void TrainingProgramUi::addProgramNameTextBox()
	{
		// Note: ## hides the label
		if (ImGui::InputText("##name", &_programNameCache))
		{
			_renameProgramFunc(_trainingProgramId, _programNameCache);
		}
	}
	void TrainingProgramUi::addProgramDurationLabel()
	{
		std::chrono::minutes durationSum;
		for (const auto& duration : _durationCache)
		{
			durationSum += std::chrono::minutes(duration);
		}
		auto durationString = fmt::format("{:>3} min", durationSum.count()); // minutes max 3 digits, right aligned
		ImGui::TextUnformatted(durationString.c_str());
	}
	void TrainingProgramUi::addProgramEntryNameTextBox(uint16_t index)
	{
		if (ImGui::InputText(fmt::format("##entryname_{}", index).c_str(), &_entryNameCache[index]))
		{
			_renameEntryFunc(_trainingProgramId, index, _entryNameCache[index]);
		}
	}
	void TrainingProgramUi::addProgramEntryDurationTextBox(uint16_t index)
	{
		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputInt(fmt::format("##entryduration_{}", index).c_str(), &_durationCache[index]))
		{
			const auto durationInMinutes = std::chrono::minutes(_durationCache[index]);
			const auto durationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationInMinutes);
			_changeDurationFunc(_trainingProgramId, index, durationInMilliseconds);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::TextUnformatted("min");
	}
	void TrainingProgramUi::addUpButton(uint16_t index, bool hasPreviousEntry)
	{
		ImGui::Disable(!hasPreviousEntry);
		if (ImGui::ArrowButton(fmt::format("##entryup_{}", index).c_str(), ImGuiDir_Up) && hasPreviousEntry)
		{
			_swapEntriesFunc(_trainingProgramId, index, index - 1);
		}
	}
	void TrainingProgramUi::addDownButton(uint16_t index, bool hasNextEntry)
	{
		ImGui::Disable(!hasNextEntry);
		if (ImGui::ArrowButton(fmt::format("##entrydown_{}", index).c_str(), ImGuiDir_Down) && hasNextEntry)
		{
			_swapEntriesFunc(_trainingProgramId, index, index + 1);
		}
	}
	void TrainingProgramUi::addDeleteButton(uint16_t index)
	{
		if (ImGui::Button(fmt::format("##entrydelete_{}", index).c_str(), "Delete"))
		{
			_removeEntryFunc(_trainingProgramId, index);
		}
	}
	void TrainingProgramUi::addAddButton()
	{
		if (ImGui::Button("##entryadd", "Add"))
		{
			_addEntryFunc(_trainingProgramId);
		}
	}
}