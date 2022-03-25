#include <pch.h>
#include "TrainingProgramListUi.h"
#include <IMGUI/imgui_stdlib.h>

namespace Ui
{
    void TrainingProgramListUi::initialize(std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> trainingProgramList)
    {
        _trainingProgramList = trainingProgramList;
        updateFromList();
    }

    void TrainingProgramListUi::renderTrainingProgramList()
    {
        ImGui::TextUnformatted("Available Training programs");

        int count = 0;
        auto entriesCopy = std::vector<Core::Configuration::Domain::TrainingProgramListEntry>(_currentEntries);
        for (auto entryIter = entriesCopy.cbegin(); entryIter != entriesCopy.cend(); entryIter++)
        {
            auto entry = *entryIter;
            addProgramNameTextBox(count, entry);
            ImGui::SameLine();
            addProgramDurationLabel(entry);
            ImGui::SameLine();
            addUpButton(count, entry, (entryIter != entriesCopy.cbegin() ? &*(entryIter - 1) : nullptr));
            ImGui::SameLine();
            addDownButton(count, entry, (entryIter + 1 != entriesCopy.cend()) ? &*(entryIter + 1) : nullptr);
            ImGui::SameLine();
            addEditButton(count, entry);
            ImGui::SameLine();
            addDeleteButton(count, entry);

            count++;
        }

        addAddButton();
    }

    void TrainingProgramListUi::addProgramNameTextBox(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry)
    {
        // Note: ## hides the label
        if (ImGui::InputText(fmt::format("##name{}", lineNumber).c_str(), &_entryNameCache[entry.TrainingProgramId]))
        {
            _trainingProgramList->renameTrainingProgram(entry.TrainingProgramId, _entryNameCache[entry.TrainingProgramId]);
        }
    }

    void TrainingProgramListUi::addProgramDurationLabel(const Core::Configuration::Domain::TrainingProgramListEntry& entry)
    {
        auto durationInMinutes = entry.TrainingProgramDuration / 60000; // intended integer division!
        auto durationString = fmt::format("{:>3} min", durationInMinutes); // minutes max 3 digits, right aligned
        ImGui::TextUnformatted(durationString.c_str());
    }

    void TrainingProgramListUi::addUpButton(
        int lineNumber,
        const Core::Configuration::Domain::TrainingProgramListEntry& entry,
        const Core::Configuration::Domain::TrainingProgramListEntry* const previousEntry)
    {
        if (ImGui::ArrowButton(fmt::format("##up_{}", lineNumber).c_str(), ImGuiDir_Up))
        {
            if (previousEntry != nullptr)
            {
                _trainingProgramList->swapTrainingPrograms(
                    entry.TrainingProgramId,
                    previousEntry->TrainingProgramId
                );
                updateFromList();
            }
        }
    }
    void TrainingProgramListUi::addDownButton(
        int lineNumber,
        const Core::Configuration::Domain::TrainingProgramListEntry& entry,
        const Core::Configuration::Domain::TrainingProgramListEntry* const nextEntry)
    {
        if (ImGui::ArrowButton(fmt::format("##down_{}", lineNumber).c_str(), ImGuiDir_Down))
        {
            if (nextEntry != nullptr)
            {
                _trainingProgramList->swapTrainingPrograms(
                    entry.TrainingProgramId,
                    nextEntry->TrainingProgramId
                );
                updateFromList();
            }
        }
    }

    void TrainingProgramListUi::addEditButton(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry)
    {
        if (ImGui::Button(fmt::format("##edit_{}", lineNumber).c_str(), "Edit"))
        {
            // TODO - call some callback so the calling class knows it's supposed to switch to rendering a single program
            
        }
    }

    void TrainingProgramListUi::addDeleteButton(int lineNumber, const Core::Configuration::Domain::TrainingProgramListEntry& entry)
    {
        if (ImGui::Button(fmt::format("##delete_{}", lineNumber).c_str(), "Delete"))
        {
            _trainingProgramList->removeTrainingProgram(entry.TrainingProgramId);
            updateFromList();
        }
    }

    void TrainingProgramListUi::addAddButton()
    {
        if (ImGui::Button("Add"))
        {
            // TEMP
            static uint64_t newId = 1000;
            _trainingProgramList->addTrainingProgram(newId);
            _trainingProgramList->getTrainingProgram(newId)->renameProgram("New Program");
            updateFromList();
            newId++;
        }
    }
    void TrainingProgramListUi::updateFromList()
    {
        _entryNameCache.clear();
        _currentEntries = _trainingProgramList->getListEntries();
        for (const auto& entry : _currentEntries)
        {
            _entryNameCache.emplace(entry.TrainingProgramId, entry.TrainingProgramName);
        }
    }
}