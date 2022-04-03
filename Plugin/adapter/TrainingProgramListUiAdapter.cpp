#include <pch.h>
#include "TrainingProgramListUiAdapter.h"

namespace Adapter
{
    TrainingProgramListUiAdapter::TrainingProgramListUiAdapter()
        : BakkesMod::Plugin::PluginSettingsWindow()
    {
    }

    void TrainingProgramListUiAdapter::connectToAppService(std::shared_ptr<Core::Configuration::Application::TrainingProgramConfigurationService> appService)
    {
        // Function for switching the editing a single training program
        auto startEditingTrainingProgram = [this](uint64_t trainingProgramId)
        {
            _trainingProgramUi->setCurrentTrainingProgramId(trainingProgramId);
            _isEditing = true;
        };
        // Function for switching back to editing the list of training programs
        auto finishEditingTrainingProgram = [this]() {
            _isEditing = false;
        };
        // Function for adding a training program
        auto addTrainingProgramFunc = [this]() {
            _appService->addTrainingProgram({});
        };
        // Function for removing a training program
        auto removeTrainingProgramFunc = [this](uint64_t id) {
            _appService->removeTrainingProgram({ id });
        };
        // Function for renaming a training program
        auto renameTrainingProgramFunc = [this](uint64_t id, const std::string& name) {
            _appService->renameTrainingProgram({ id, name });
        };
        // Function for swapping two training programs
        auto swapTrainingProgramsFunc = [this](uint64_t firstId, uint64_t secondId) {
            _appService->swapTrainingPrograms({ firstId, secondId });
        };
        // Function for adding a training program entry
        auto addEntryFunc = [this](uint64_t programId) {
            _appService->addTrainingProgramEntry({ programId });
        };
        // Function for removing a training program entry
        auto removeEntryFunc = [this](uint64_t programId, uint16_t position) {
            _appService->removeTrainingProgramEntry({ programId, position });
        };
        // Function for swapping two training program entries
        auto swapEntriesFunc = [this](uint64_t programId, uint16_t firstPosition, uint16_t secondPosition) {
            _appService->swapTrainingProgramEntries({ programId, firstPosition, secondPosition });
        };
        // Function for renaming a training program entry
        auto renameEntryFunc = [this](uint64_t programId, uint16_t position, const std::string& entryName) {
            _appService->renameTrainingProgramEntry({ programId, position, entryName });
        };
        // Function for changing the duration of a training program entry
        auto changeDurationFunc = [this](uint64_t programId, uint16_t position, const std::chrono::milliseconds& duration) {
            _appService->changeTrainingProgramEntryDuration({ programId, position, duration });
        };


        // Now that all adapter functions have been defined, we can connect the UI and the app service without them knowing each other
        // There's not too much benefit in this project, but in theory we could even put those classes into separate services and connect them via a REST API or something.
        _trainingProgramUi = std::make_shared<Ui::TrainingProgramUi>(
            finishEditingTrainingProgram,
            renameTrainingProgramFunc,
            addEntryFunc,
            removeEntryFunc,
            renameEntryFunc,
            changeDurationFunc,
            swapEntriesFunc);
        _trainingProgramListUi = std::make_shared<Ui::TrainingProgramListUi>(
            startEditingTrainingProgram,
            addTrainingProgramFunc,
            removeTrainingProgramFunc,
            renameTrainingProgramFunc,
            swapTrainingProgramsFunc);
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

    void TrainingProgramListUiAdapter::processEvent(const std::shared_ptr<Core::Kernel::DomainEvent>& genericEvent)
    {
        
        if (auto programListChangeEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramListChangedEvent>(genericEvent); 
            programListChangeEvent != nullptr)
        {
            _trainingProgramListUi->adaptToEvent(programListChangeEvent);
        }
        else if (auto programChangeEvent = std::dynamic_pointer_cast<Core::Configuration::Events::TrainingProgramChangedEvent>(genericEvent);
                 programChangeEvent != nullptr)
        {
            _trainingProgramUi->adaptToEvent(programChangeEvent);
        }
    }
}