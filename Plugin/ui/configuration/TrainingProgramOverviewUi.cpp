#include <pch.h>
#include "TrainingProgramOverviewUi.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/imgui_disable.h>

namespace Ui
{
    TrainingProgramOverviewUi::TrainingProgramOverviewUi(
        std::shared_ptr<GameWrapper> gameWrapper,
        std::function<void(uint64_t)> startEditingFunc,
        std::function<void()> addTrainingProgramFunc,
        std::function<void(uint64_t)> removeTrainingProgramFunc,
        std::function<void(uint64_t, const std::string&)> renameTrainingProgramFunc,
        std::function<void(uint64_t, uint64_t)> swapTrainingProgramsFunc)
        // Note: Why do we ask for a copy and then std::move it, rather than just asking for a reference?
        // The reason is that this way the caller of this constructor knows that we are going to store the object.
        // If the caller provides an RValue to the constructor, the compiler might even elide the copy made in the constructor.
        // Therefore we have an expressive interface ("Express what you intend"), while not making unnecessary copies.
        : _gameWrapper{ gameWrapper }
        , _startEditingFunc{ std::move(startEditingFunc) }
        , _addTrainingProgramFunc{ std::move(addTrainingProgramFunc) }
        , _removeTrainingProgramFunc{ std::move(removeTrainingProgramFunc) }
        , _renameTrainingProgramFunc{ std::move(renameTrainingProgramFunc) }
        , _swapTrainingProgramsFunc{ std::move(swapTrainingProgramsFunc) }
    {
    }

    void TrainingProgramOverviewUi::renderTrainingProgramList()
    {
        ImGui::TextUnformatted("Training control");
        addTrainingControlWindowButton();
        ImGui::Separator();

        ImGui::TextUnformatted("Available Training programs");

        addAddButton();

        if (_mostRecentChangeEvent == nullptr) { return; }

        const auto trainingProgramList = std::vector<Core::Configuration::Events::TrainingProgramInfo>(_mostRecentChangeEvent->TrainingProgramListInfo);
        auto numberOfPrograms = trainingProgramList.size();
        for (auto index = (uint16_t)0; index < (uint16_t)numberOfPrograms; index++)
        {
            const auto& trainingProgramInfo = trainingProgramList.at(index);
            addProgramNameTextBox(index, trainingProgramInfo);
            ImGui::SameLine();
            addProgramDurationLabel(trainingProgramInfo);
            ImGui::SameLine();
            addUpButton(index, trainingProgramInfo, (index > 0 ? &trainingProgramList.at(index - 1) : nullptr));
            ImGui::SameLine();
            addDownButton(index, trainingProgramInfo, (index < numberOfPrograms - 1 ? &trainingProgramList.at(index + 1) : nullptr));
            ImGui::SameLine();
            addEditButton(index, trainingProgramInfo);
            ImGui::SameLine();
            addDeleteButton(index, trainingProgramInfo);
        }
    }

    void TrainingProgramOverviewUi::adaptToEvent(const std::shared_ptr<Core::Configuration::Events::TrainingProgramListChangedEvent>& changeEvent)
    {
        _mostRecentChangeEvent = changeEvent;

        // We need to update the cache for the Text controls of IMGUI
        _entryNameCache.clear();
        for (auto info : changeEvent->TrainingProgramListInfo)
        {
            _entryNameCache.emplace(info.Id, info.Name);
        }
    }

    void TrainingProgramOverviewUi::addTrainingControlWindowButton()
    {
        if (ImGui::Button("Open Training Window"))
        {
            _gameWrapper->Execute([](GameWrapper*) {
                _globalCvarManager->executeCommand("togglemenu rltrainingtimer");
            });
        }
    }

    void TrainingProgramOverviewUi::addProgramNameTextBox(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info)
    {
        // Note: ## hides the label
        if (ImGui::InputText(fmt::format("##name{}", index).c_str(), &_entryNameCache[info.Id]))
        {
            _renameTrainingProgramFunc(info.Id, _entryNameCache[info.Id]);
        }
    }

    void TrainingProgramOverviewUi::addProgramDurationLabel(const Core::Configuration::Events::TrainingProgramInfo& info)
    {
        auto durationString = fmt::format("{:>3} min", std::chrono::duration_cast<std::chrono::minutes>(info.Duration).count()); // minutes max 3 digits, right aligned
        ImGui::TextUnformatted(durationString.c_str());
    }

    void TrainingProgramOverviewUi::addUpButton(
        uint16_t index,
        const Core::Configuration::Events::TrainingProgramInfo& info,
        const Core::Configuration::Events::TrainingProgramInfo* const previousInfo)
    {
        ImGui::Disable disable(previousInfo == nullptr);
        if (ImGui::ArrowButton(fmt::format("##up_{}", index).c_str(), ImGuiDir_Up) && previousInfo != nullptr)
        {
            _swapTrainingProgramsFunc(info.Id, previousInfo->Id);
        }
    }
    void TrainingProgramOverviewUi::addDownButton(
        uint16_t index,
        const Core::Configuration::Events::TrainingProgramInfo& info,
        const Core::Configuration::Events::TrainingProgramInfo* const nextInfo)
    {
        ImGui::Disable disable(nextInfo == nullptr);
        if (ImGui::ArrowButton(fmt::format("##down_{}", index).c_str(), ImGuiDir_Down) && nextInfo != nullptr)
        {
            _swapTrainingProgramsFunc(info.Id, nextInfo->Id);
        }
    }

    void TrainingProgramOverviewUi::addEditButton(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info)
    {
        if (ImGui::Button(fmt::format("##edit_{}", index).c_str(), "Edit"))
        {
            _startEditingFunc(info.Id);
        }
    }

    void TrainingProgramOverviewUi::addDeleteButton(uint16_t index, const Core::Configuration::Events::TrainingProgramInfo& info)
    {
        if (ImGui::Button(fmt::format("##delete_{}", index).c_str(), "Delete"))
        {
            _removeTrainingProgramFunc(info.Id);
        }
    }

    void TrainingProgramOverviewUi::addAddButton()
    {
        if (ImGui::Button("Add"))
        {
            _addTrainingProgramFunc();
        }
    }
}