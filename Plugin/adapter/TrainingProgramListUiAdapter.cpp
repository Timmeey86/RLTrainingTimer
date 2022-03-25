#include <pch.h>
#include "TrainingProgramListUiAdapter.h"

namespace Adapter
{
    TrainingProgramListUiAdapter::TrainingProgramListUiAdapter()
        : BakkesMod::Plugin::PluginSettingsWindow()
        , _trainingProgramListUi{ std::make_shared<Ui::TrainingProgramListUi>() }
    {
        
    }

    void TrainingProgramListUiAdapter::RenderSettings()
    {
        auto notEditing = true; 
        if (notEditing)
        {
            _trainingProgramListUi->renderTrainingProgramList();
        }
        else
        {
            // TODO: Render page for a single program
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
        _trainingProgramListUi->initialize(trainingProgramList);
    }

    void TrainingProgramListUiAdapter::unsubscribe()
    {
        // TODO: Unsubscribe from events
    }
}