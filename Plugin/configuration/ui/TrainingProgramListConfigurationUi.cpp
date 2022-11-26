#include <pch.h>
#include "TrainingProgramListConfigurationUi.h"
#include "file_dialogs.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/imgui_disable.h>

namespace configuration
{
    TrainingProgramListConfigurationUi::TrainingProgramListConfigurationUi(
        std::shared_ptr<GameWrapper> gameWrapper,
        std::shared_ptr<TrainingProgramListConfigurationControl> listConfigurationControl,
        std::shared_ptr<TrainingProgramConfigurationControl> programConfigurationControl,
        std::function<void(const std::string&)> startEditingCallback)
        : _gameWrapper{ gameWrapper }
        , _listConfigurationControl{ listConfigurationControl }
        , _programConfigurationControl{ programConfigurationControl }
        , _startEditingCallback{ std::move(startEditingCallback) }
    {
    }

    void TrainingProgramListConfigurationUi::renderTrainingProgramList()
    {
        ImGui::TextUnformatted("Training control");
        addTrainingControlWindowButton();
        addWorkshopFolderLocationTextBos();

        ImGui::Separator();

        ImGui::TextUnformatted("Backup");
		addLoadSaveButtons();

        ImGui::Separator();

        ImGui::TextUnformatted("Available Training programs");


        bool listHasChanged = false;
        
        listHasChanged |= addAddButton();
		ImGui::SameLine();
		listHasChanged |= addLoadButton();

        // TODO: Store this in a cache and update only when something in here changes, or when returning from the single training program screen, and after initially loading data
        const auto data = _listConfigurationControl->getTrainingProgramList();
        _entryNameCache.clear();
        for (auto [id, trainingProgramData] : data.TrainingProgramData)
        {
            _entryNameCache.emplace(id, trainingProgramData.Name);
        }
        _workshopFolderLocation = data.WorkshopFolderLocation;

        auto numberOfPrograms = data.TrainingProgramOrder.size();
        for (auto index = (uint16_t)0; index < (uint16_t)numberOfPrograms; index++)
        {
            const auto& trainingProgramId = data.TrainingProgramOrder[index];
            const auto& trainingProgramInfo = data.TrainingProgramData.at(trainingProgramId);
            const auto previousProgramId = (index > 0 ? &data.TrainingProgramOrder.at(index - 1) : nullptr);
            const auto nextProgramId = (index < numberOfPrograms - 1 ? &data.TrainingProgramOrder.at(index + 1) : nullptr);
            const auto previousData = previousProgramId == nullptr ? nullptr : &data.TrainingProgramData.at(*previousProgramId);
            const auto nextData = nextProgramId == nullptr ? nullptr : &data.TrainingProgramData.at(*nextProgramId);

            listHasChanged |= addProgramNameTextBox(index, trainingProgramInfo);
            ImGui::SameLine();
            addProgramDurationLabel(trainingProgramInfo);
            ImGui::SameLine();
            listHasChanged |= addUpButton(index, trainingProgramInfo, previousData);
            ImGui::SameLine();
            listHasChanged |= addDownButton(index, trainingProgramInfo, nextData);
            ImGui::SameLine();
            addEditButton(index, trainingProgramInfo);
            ImGui::SameLine();
            listHasChanged |= addDeleteButton(index, trainingProgramInfo);
			ImGui::SameLine();
            listHasChanged |= addSaveButton(index, trainingProgramInfo);
        }

        if (listHasChanged)
        {
            // TODO: Update cache here
            // TODO: Trigger writing, maybe in the control classes
        }
    }

    void TrainingProgramListConfigurationUi::addWorkshopFolderLocationTextBos()
    {
        if (ImGui::InputText("Workshop Folder Location##workshoplocation", &_workshopFolderLocation))
        {
            _listConfigurationControl->changeWorkshopFolderLocation(_workshopFolderLocation);
        }
    }
    void TrainingProgramListConfigurationUi::addTrainingControlWindowButton()
    {
        if (ImGui::Button("Open Training Window"))
        {
            _gameWrapper->Execute([](GameWrapper*) {
                _globalCvarManager->executeCommand("togglemenu rltrainingtimer");
            });
        }
    }

	void TrainingProgramListConfigurationUi::addLoadSaveButtons()
	{

		if(ImGui::Button("Export All"))
		{
            auto path = file_dialogs::getOpenFilePath("", { "json" });
			_listConfigurationControl->restoreWholeTrainingProgramList(path.string());
		}

		ImGui::SameLine();
		if(ImGui::Button("Import and Override All"))
		{
            auto path = file_dialogs::getSaveFilePath("", { "json" });
			_listConfigurationControl->storeWholeTrainingProgramList(path.string());
		}
	}

    bool TrainingProgramListConfigurationUi::addProgramNameTextBox(uint16_t index, const TrainingProgramData& info)
    {
        // Note: ## hides the label
        if (ImGui::InputText(fmt::format("##name{}", index).c_str(), &_entryNameCache[info.Id]))
        {
            _programConfigurationControl->renameProgram(info.Id, _entryNameCache[info.Id]);
            return true;
        }
        return false;
    }

    void TrainingProgramListConfigurationUi::addProgramDurationLabel(const TrainingProgramData& info)
    {
        auto durationString = fmt::format("{:>3} min", std::chrono::duration_cast<std::chrono::minutes>(info.Duration).count()); // minutes max 3 digits, right aligned
        ImGui::TextUnformatted(durationString.c_str());
    }

    bool TrainingProgramListConfigurationUi::addUpButton(
        uint16_t index,
        const TrainingProgramData& info,
        const TrainingProgramData* const previousInfo)
    {
        ImGui::Disable disable(previousInfo == nullptr);
        if (ImGui::ArrowButton(fmt::format("##up_{}", index).c_str(), ImGuiDir_Up) && previousInfo != nullptr)
        {
            _listConfigurationControl->swapTrainingPrograms(info.Id, previousInfo->Id);
            return true;
        }
        return false;
    }
    bool TrainingProgramListConfigurationUi::addDownButton(
        uint16_t index,
        const TrainingProgramData& info,
        const TrainingProgramData* const nextInfo)
    {
        ImGui::Disable disable(nextInfo == nullptr);
        if (ImGui::ArrowButton(fmt::format("##down_{}", index).c_str(), ImGuiDir_Down) && nextInfo != nullptr)
        {
            _listConfigurationControl->swapTrainingPrograms(info.Id, nextInfo->Id);
            return true;
        }
        return false;
    }

    void TrainingProgramListConfigurationUi::addEditButton(uint16_t index, const TrainingProgramData& info)
    {
        if (ImGui::Button(fmt::format("##edit_{}", index).c_str(), "Edit"))
        {
            _startEditingCallback(info.Id);
        }
    }

    bool TrainingProgramListConfigurationUi::addDeleteButton(uint16_t index, const TrainingProgramData& info)
    {
        if (ImGui::Button(fmt::format("##delete_{}", index).c_str(), "Delete"))
        {
            _listConfigurationControl->removeTrainingProgram(info.Id);
            return true;
        }
        return false;
    }

	bool TrainingProgramListConfigurationUi::addSaveButton(uint16_t index, const TrainingProgramData& info)
    {
        if (ImGui::Button(fmt::format("##save_{}", index).c_str(), "Export"))
        {
            auto path = file_dialogs::getSaveFilePath("", { "json" });
            if (!path.string().ends_with(".json"))
            {
                path += ".json";
            }
            _listConfigurationControl->exportSingleTrainingProgram(info.Id, path.string());
            return true;
        }
        return false;
    }

    bool TrainingProgramListConfigurationUi::addAddButton()
    {
        if (ImGui::Button("Add"))
        {
            _listConfigurationControl->addTrainingProgram();
            return true;
        }
        return false;
    }
	
	bool TrainingProgramListConfigurationUi::addLoadButton()
    {
        if (ImGui::Button("Import from file"))
        {
            auto path = file_dialogs::getOpenFilePath("", { "json" });
            _listConfigurationControl->importSingleTrainingProgram(path.string());
            return true;
        }
        return false;
    }

}