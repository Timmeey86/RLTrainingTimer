#include <pch.h>
#include "ConfigurationUi.h"
#include "TrainingProgramConfigurationUi.h"
#include "TrainingProgramListConfigurationUi.h"

namespace configuration
{
    void ConfigurationUi::initConfigurationUi(
        std::shared_ptr<GameWrapper> gameWrapper,
        std::shared_ptr<TrainingProgramConfigurationControl> singleProgramControl, 
        std::shared_ptr<TrainingProgramListConfigurationControl> programListControl)
    {
        // Function to be called when pressing "Edit" on a training program
        auto switchToEditingFunc = [this](const std::string& trainingProgramId) {
            _singleTrainingProgramUi->setCurrentTrainingProgramId(trainingProgramId);
            _isEditing = true;
        };

        // Function to be called when pressing "Back" on a training program
        auto returnToOverviewFunc = [this]() { _isEditing = false; };

        _singleTrainingProgramUi = std::make_shared<TrainingProgramConfigurationUi>(singleProgramControl, returnToOverviewFunc);
        _trainingProgramOverviewUi = std::make_shared<TrainingProgramListConfigurationUi>(gameWrapper, programListControl, singleProgramControl, switchToEditingFunc);
    }

    void ConfigurationUi::RenderSettings()
    {
        if (_isEditing)
        {
            _singleTrainingProgramUi->renderTrainingProgram();
        }
        else
        {
            _trainingProgramOverviewUi->renderTrainingProgramList();
        }
    }


    std::string ConfigurationUi::GetPluginName()
    {
        return "RL Training Timer";
    }

    void ConfigurationUi::SetImGuiContext(uintptr_t ctx)
    {
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
    }
}