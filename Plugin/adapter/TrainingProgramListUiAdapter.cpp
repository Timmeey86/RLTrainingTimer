#include <pch.h>
#include "TrainingProgramListUiAdapter.h"
#include <IMGUI/imgui_stdlib.h>

namespace Adapter
{
    void TrainingProgramListUiAdapter::RenderSettings()
    {
        ImGui::TextUnformatted("Plugin settings");

        int count = 0;
        auto entriesCopy = std::vector<Core::Configuration::Domain::TrainingProgramListEntry>(_currentEntries);
        for(auto entryIter = entriesCopy.cbegin(); entryIter != entriesCopy.cend(); entryIter++)
        {
            auto entry = *entryIter;
            if (ImGui::InputText(fmt::format("#{}", count).c_str(), &_entryNameCache[entry.TrainingProgramId]))
            {
                _trainingProgramList->renameTrainingProgram(entry.TrainingProgramId, _entryNameCache[entry.TrainingProgramId]);
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(std::to_string((float)entry.TrainingProgramDuration / 1000.0f).c_str());
            ImGui::SameLine();
            if (ImGui::ArrowButton(fmt::format("##up_{}", count).c_str(), ImGuiDir_Up))
            {
                if (entryIter != entriesCopy.cbegin())
                {
                    auto previousEntry = *(entryIter - 1);
                    _trainingProgramList->swapTrainingPrograms(
                        entry.TrainingProgramId,
                        previousEntry.TrainingProgramId
                    );
                    updateFromList();
                }
            }
            ImGui::SameLine();
            if (ImGui::ArrowButton(fmt::format("##down_{}", count).c_str(), ImGuiDir_Down))
            {
                auto nextIter = entryIter + 1;
                if (nextIter != entriesCopy.cend())
                {
                    auto nextEntry = *nextIter;
                    _trainingProgramList->swapTrainingPrograms(
                        entry.TrainingProgramId,
                        nextEntry.TrainingProgramId
                    );
                    updateFromList();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(fmt::format("##edit_{}", count).c_str(), "Edit"))
            {
                // TODO
                updateFromList();
            }
            ImGui::SameLine();
            if (ImGui::Button(fmt::format("##delete_{}", count).c_str(), "Delete"))
            {
                _trainingProgramList->removeTrainingProgram(entry.TrainingProgramId);
                updateFromList();
            }

            count++;
        }

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

    std::string TrainingProgramListUiAdapter::GetPluginName()
    {
        return "RL Training Timer";
    }

    void TrainingProgramListUiAdapter::SetImGuiContext(uintptr_t ctx)
    {
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
    }

    void TrainingProgramListUiAdapter::subscribe(std::shared_ptr<Core::Configuration::Domain::TrainingProgramList> trainingProgramList)
    {
        // TODO: Subscribe to events
        _trainingProgramList = trainingProgramList;
        updateFromList();
    }

    void TrainingProgramListUiAdapter::unsubscribe()
    {
        // TODO: Unsubscribe from events
        _currentEntries.clear();
        _entryNameCache.clear();
    }
    void TrainingProgramListUiAdapter::updateFromList()
    {
        _entryNameCache.clear();
        _currentEntries = _trainingProgramList->getListEntries();
        for (const auto& entry : _currentEntries)
        {
            _entryNameCache.emplace(entry.TrainingProgramId, entry.TrainingProgramName);
        }
    }
}