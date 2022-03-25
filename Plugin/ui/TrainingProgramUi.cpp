#include <pch.h>
#include "TrainingProgramUi.h"
#include <IMGUI/imgui_stdlib.h>

namespace Ui
{
	TrainingProgramUi::TrainingProgramUi(std::function<void()> finishEditingFunc)
		: _finishEditingFunc{ finishEditingFunc }
	{

	}
	void TrainingProgramUi::setCurrentTrainingProgram(std::shared_ptr<Core::Configuration::Domain::TrainingProgram> trainingProgram)
	{
		_trainingProgram = trainingProgram;
		updateCaches();
	}

	void TrainingProgramUi::updateCaches()
	{
		_entryNameCache.clear();
		_durationCache.clear();

		if (_trainingProgram != nullptr)
		{
			_programNameCache = _trainingProgram->name();
			const auto entries = _trainingProgram->entries();
			for (const auto& entry : entries)
			{
				_entryNameCache.push_back(entry.name());
				_durationCache.push_back(entry.duration() / 60000); // minutes
			}
		}
	}
	void TrainingProgramUi::renderTrainingProgram()
	{
		if (_trainingProgram == nullptr) { return; }

		addBackButton();
		ImGui::Separator();
		ImGui::TextUnformatted("Training Summary");
		addProgramNameTextBox();
		ImGui::SameLine();
		addProgramDurationLabel();
		ImGui::Separator();
		ImGui::TextUnformatted("Training steps");
		
		auto entriesCopy = std::vector<Core::Configuration::Domain::TrainingProgramEntry>(_trainingProgram->entries());
		int count = 0;
		for (auto entryIter = entriesCopy.cbegin(); entryIter != entriesCopy.cend(); entryIter++)
		{
			const auto& entry = *entryIter;
			addProgramEntryNameTextBox(count, entry);
			ImGui::SameLine();
			addProgramEntryDurationTextBox(count, entry);
			ImGui::SameLine();
			addUpButton(count, count > 0);
			ImGui::SameLine();
			addDownButton(count, count < entriesCopy.size());
			ImGui::SameLine();
			addDeleteButton(count);

			count++;
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
			_trainingProgram->renameProgram(_programNameCache);
		}
	}
	void TrainingProgramUi::addProgramDurationLabel()
	{
		auto durationInMinutes = _trainingProgram->programDuration() / 60000U; // intended integer division!
		auto durationString = fmt::format("{:>3} min", durationInMinutes); // minutes max 3 digits, right aligned
		ImGui::TextUnformatted(durationString.c_str());
	}
	void TrainingProgramUi::addProgramEntryNameTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramEntry& entry)
	{
		if (ImGui::InputText(fmt::format("##entryname_{}", lineNumber).c_str(), &_entryNameCache[lineNumber]))
		{
			_trainingProgram->replaceEntry(lineNumber, { _entryNameCache[lineNumber], entry.duration() });
		}
	}
	void TrainingProgramUi::addProgramEntryDurationTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramEntry& entry)
	{
		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputInt(fmt::format("##entryduration_{}", lineNumber).c_str(), &_durationCache[lineNumber]))
		{
			if (_durationCache[lineNumber] < 0)
			{
				_durationCache[lineNumber] = 0;
			}
			else
			{
				_trainingProgram->replaceEntry(lineNumber, { entry.name(), _durationCache[lineNumber] * 60000U });
			}
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::TextUnformatted("min");
	}
	void TrainingProgramUi::addUpButton(int lineNumber, bool hasPreviousEntry)
	{
		if (ImGui::ArrowButton(fmt::format("##entryup_{}", lineNumber).c_str(), ImGuiDir_Up))
		{
			if (hasPreviousEntry)
			{
				_trainingProgram->swapEntries(lineNumber, lineNumber - 1);
				updateCaches();
			}
		}
	}
	void TrainingProgramUi::addDownButton(int lineNumber, bool hasNextEntry)
	{
		if (ImGui::ArrowButton(fmt::format("##entrydown_{}", lineNumber).c_str(), ImGuiDir_Down))
		{
			if (hasNextEntry)
			{
				_trainingProgram->swapEntries(lineNumber, lineNumber + 1);
				updateCaches();
			}
		}
	}
	void TrainingProgramUi::addDeleteButton(int lineNumber)
	{
		if (ImGui::Button(fmt::format("##entrydelete_{}", lineNumber).c_str(), "Delete"))
		{
			_trainingProgram->removeEntry(lineNumber);
			updateCaches();
		}
	}
	void TrainingProgramUi::addAddButton()
	{
		if (ImGui::Button("##entryadd", "Add"))
		{
			_trainingProgram->addEntry({ "New", 0 });
			updateCaches();
		}
	}
}