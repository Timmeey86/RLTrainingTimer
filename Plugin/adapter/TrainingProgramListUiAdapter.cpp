#include "pch.h"
#include "TrainingProgramListUiAdapter.h"

namespace Adapter
{
    void TrainingProgramListUiAdapter::RenderSettings()
    {
        ImGui::TextUnformatted("Plugin settings");

        int count = 0;
        for(auto entryIter = _currentEntries.cbegin(); entryIter != _currentEntries.cend(); entryIter++)
        {
            auto entry = *entryIter;
            ImGui::TextUnformatted(entry.TrainingProgramName.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(std::to_string((float)entry.TrainingProgramDuration / 1000.0f).c_str());
            ImGui::SameLine();
            if (ImGui::ArrowButton(fmt::format("##up_{}", count).c_str(), ImGuiDir_Up))
            {
                // TODO
            }
            ImGui::SameLine();
            if (ImGui::ArrowButton(fmt::format("##down_{}", count).c_str(), ImGuiDir_Down))
            {
                // TODO
            }
            ImGui::SameLine();
            if (ImGui::Button("Edit"))
            {
                // TODO
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                // TODO
            }

            count++;
        }

        if (ImGui::Button("Add")) 
        {
            // TODO
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
        _currentEntries = _trainingProgramList->getListEntries();
    }

    void TrainingProgramListUiAdapter::unsubscribe()
    {
        // TODO: Unsubscribe from events
        _currentEntries.clear();
    }
}