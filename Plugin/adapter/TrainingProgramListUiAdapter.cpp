#include <pch.h>
#include "TrainingProgramListUiAdapter.h"

namespace Adapter
{
    TrainingProgramListUiAdapter::TrainingProgramListUiAdapter()
        : BakkesMod::Plugin::PluginSettingsWindow()
    {
        auto startEditingTrainingProgram = [this](const std::shared_ptr<Core::Configuration::Domain::TrainingProgram>& trainingProgram)
        {
            _trainingProgramUi->setCurrentTrainingProgram(trainingProgram);
            _isEditing = true;
        };
        auto finishEditingTrainingProgram = [this]() {
            _isEditing = false;
            _trainingProgramListUi->updateFromList();
        };
        _trainingProgramUi = std::make_shared<Ui::TrainingProgramUi>(finishEditingTrainingProgram);
        _trainingProgramListUi = std::make_shared<Ui::TrainingProgramListUi>(startEditingTrainingProgram);
    }

    void TrainingProgramListUiAdapter::RenderSettings()
    {
        if (_isEditing)
        {
            _trainingProgramUi->renderTrainingProgram();
        }
        else
        {
            _trainingProgramListUi->renderTrainingProgramList();
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
        _trainingProgramList = trainingProgramList;
        _trainingProgramListUi->initialize(trainingProgramList);

        // TODO: Subscribe to events in order to store them on the disk. Use the nlohmann json library for this (in an own class).
        // TODO: Restore events after initializing and apply the to the training program list.
    }

    void TrainingProgramListUiAdapter::unsubscribe()
    {
    }
}