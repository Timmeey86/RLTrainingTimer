#include <pch.h>
#include "TrainingProgramFlowControlUi.h"

#include <external/IMGUI/imgui.h>
#include <external/IMGUI/imgui_disable.h>

namespace Ui
{
	void TrainingProgramFlowControlUi::initTrainingProgramFlowControlUi(std::shared_ptr<Core::Training::Application::TrainingApplicationService> appService)
	{
		_appService = appService;
	}
	void TrainingProgramFlowControlUi::Render()
	{
		if (!_isWindowOpen)
		{
			toggleMenu();
			return;
		}

		if (_appService == nullptr)
		{
			return;
		}

		// Init GUI
		ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(FLT_MAX, FLT_MAX));
		ImGuiWindowFlags windowFlags = 0; // | ImGuiWindowFlags_MenuBar;
		if (!ImGui::Begin(GetMenuTitle().c_str(), &_isWindowOpen, windowFlags))
		{
			// Early out if the window is collapsed, as an optimization.
			_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
			ImGui::End();
			return;
		}

		// Render data
		renderOneFrame();

		// End GUI
		_shouldBlockInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
		ImGui::End();
	}
	std::string TrainingProgramFlowControlUi::GetMenuName()
	{
		return "rltrainingtimer";
	}
	std::string TrainingProgramFlowControlUi::GetMenuTitle()
	{
		return "RL Training Timer";
	}
	void TrainingProgramFlowControlUi::SetImGuiContext(uintptr_t ctx)
	{
		ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
	}
	bool TrainingProgramFlowControlUi::ShouldBlockInput()
	{
		return _shouldBlockInput;
	}
	bool TrainingProgramFlowControlUi::IsActiveOverlay()
	{
		return true;
	}
	void TrainingProgramFlowControlUi::OnOpen()
	{
		_isWindowOpen = true;
	}
	void TrainingProgramFlowControlUi::OnClose()
	{
		_isWindowOpen = false;
	}
	void TrainingProgramFlowControlUi::toggleMenu()
	{
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
	void TrainingProgramFlowControlUi::renderOneFrame()
	{
		// TODO: An optimization would be to cache a read model and only update it whenever there is any kind of event.
		_currentReadModel = _appService->getCurrentReadModel();

		static const char* selectedTrainingProgramName;
		if (_currentReadModel.SelectedTrainingProgramIndex.has_value())
		{
			selectedTrainingProgramName = _currentReadModel.TrainingProgramEntries[_currentReadModel.SelectedTrainingProgramIndex.value()].TrainingProgramName.c_str();
		}
		else
		{
			selectedTrainingProgramName = "Choose a program";
		}
		if (ImGui::BeginCombo("Training Program", selectedTrainingProgramName))
		{
			for (int index = 0; index < _currentReadModel.TrainingProgramEntries.size(); index++)
			{
				const auto isSelected = _currentReadModel.SelectedTrainingProgramIndex == index;
				auto trainingProgramEntry = _currentReadModel.TrainingProgramEntries[index];
				if (ImGui::Selectable(trainingProgramEntry.TrainingProgramName.c_str(), isSelected))
				{
					_appService->selectTrainingProgram({ trainingProgramEntry.TrainingProgramId });
				}
			}
			ImGui::EndCombo();
		}
		{
			ImGui::Disable disable_start_if_necessary(!_currentReadModel.StartingIsPossible);
			if (ImGui::Button("Start") && _currentReadModel.StartingIsPossible)
			{
				_appService->startTrainingProgram({});
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_pause_if_necessary(!_currentReadModel.PausingIsPossible);
			if (ImGui::Button("Pause") && _currentReadModel.PausingIsPossible)
			{
				_appService->pauseTrainingProgram({});
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_resume_if_necessary(!_currentReadModel.ResumingIsPossible);
			if (ImGui::Button("Resume") && _currentReadModel.ResumingIsPossible)
			{
				_appService->resumeTrainingProgram({});
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_stop_if_necessary(!_currentReadModel.StoppingIsPossible);
			if (ImGui::Button("Stop") && _currentReadModel.StoppingIsPossible)
			{
				_appService->abortTrainingProgram({});
			}
		}



		// For now, we'll display in the same window. We might separate this into window and canvas maybe.
		ImGui::Separator();

		if (_currentReadModel.SelectedTrainingProgramIndex.has_value())
		{
			const auto& currentTrainingProgram = _currentReadModel.TrainingProgramEntries.at(_currentReadModel.SelectedTrainingProgramIndex.value());

			auto programDuration = std::chrono::milliseconds(currentTrainingProgram.TrainingProgramDuration);
			auto minutes = std::chrono::duration_cast<std::chrono::minutes>(programDuration);
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(programDuration - minutes);
			ImGui::TextUnformatted(fmt::format("Total training program duration: {}m {}s", minutes.count(), seconds.count()).c_str()),

			minutes = std::chrono::duration_cast<std::chrono::minutes>(_currentReadModel.CurrentTrainingDuration);
			seconds = std::chrono::duration_cast<std::chrono::seconds>(_currentReadModel.CurrentTrainingDuration - minutes);
			ImGui::TextUnformatted(fmt::format("Current duration: {}m {}s", minutes.count(), seconds.count()).c_str());

			ImGui::TextUnformatted(fmt::format("Current training step: {}", _currentReadModel.CurrentTrainingStepName).c_str());
			ImGui::TextUnformatted(fmt::format("Current training step#: {}", _currentReadModel.CurrentTrainingStepNumber).c_str());
		}
		else
		{
			ImGui::TextUnformatted("No training program selected");
		}

	}
}
