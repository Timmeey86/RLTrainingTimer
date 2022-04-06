#include <pch.h>
#include "TrainingProgramFlowControlUi.h"

#include <external/IMGUI/imgui.h>
#include <external/IMGUI/imgui_disable.h>

namespace Ui
{
	void TrainingProgramFlowControlUi::initTrainingProgramFlowControlUi(
		std::shared_ptr<GameWrapper> gameWrapper,
		std::shared_ptr<Core::Training::Application::TrainingApplicationService> appService)
	{
		_appService = appService;
		gameWrapper->RegisterDrawable([this, gameWrapper](const CanvasWrapper& canvasWrapper) {
			_trainingProgramDisplay->updateReadModel(_appService->getDisplayReadModel());
			_trainingProgramDisplay->renderOneFrame(gameWrapper, canvasWrapper);
		});
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
		_currentReadModel = _appService->getFlowControlReadModel();

		if (_currentReadModel.MostRecentChangeEvent == nullptr || _currentReadModel.MostRecentStateEvent == nullptr)
		{
			return;
		}

		static const char* selectedTrainingProgramName;
		if (_currentReadModel.MostRecentSelectionEvent != nullptr)
		{
			selectedTrainingProgramName = _currentReadModel.MostRecentSelectionEvent->Name.c_str();
		}
		else
		{
			selectedTrainingProgramName = "Choose a program";
		}

		if (ImGui::BeginCombo("Training Program", selectedTrainingProgramName))
		{
			ImGui::Disable disable_selection_if_necessary(!_currentReadModel.MostRecentStateEvent->SwitchingProgramIsPossible);
			for( const auto& trainingProgramInfo : _currentReadModel.MostRecentChangeEvent->TrainingProgramListInfo)
			{
				const auto isSelected = _currentReadModel.MostRecentSelectionEvent->SelectedTrainingProgramId == trainingProgramInfo.Id;
				if (ImGui::Selectable(trainingProgramInfo.Name.c_str(), isSelected))
				{
					try
					{
						_appService->selectTrainingProgram({ trainingProgramInfo.Id });
					}
					catch (const std::runtime_error& ex)
					{
						_exceptionMessages.emplace_back(ex.what());
					}
				}
			}
			ImGui::EndCombo();
		}
		{
			ImGui::Disable disable_start_if_necessary(!_currentReadModel.MostRecentStateEvent->StartingIsPossible);
			if (ImGui::Button("Start") && _currentReadModel.MostRecentStateEvent->StartingIsPossible)
			{
				_exceptionMessages.clear();
				try
				{
					_appService->startTrainingProgram({});
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_pause_if_necessary(!_currentReadModel.MostRecentStateEvent->PausingIsPossible);
			if (ImGui::Button("Pause") && _currentReadModel.MostRecentStateEvent->PausingIsPossible)
			{
				try
				{
					_appService->pauseTrainingProgram({});
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_resume_if_necessary(!_currentReadModel.MostRecentStateEvent->ResumingIsPossible);
			if (ImGui::Button("Resume") && _currentReadModel.MostRecentStateEvent->ResumingIsPossible)
			{
				try
				{
					_appService->resumeTrainingProgram({});
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_stop_if_necessary(!_currentReadModel.MostRecentStateEvent->StoppingIsPossible);
			if (ImGui::Button("Stop") && _currentReadModel.MostRecentStateEvent->StoppingIsPossible)
			{
				try
				{
					_appService->abortTrainingProgram({});
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}

		ImGui::Separator();

		for (const auto& exceptionMessage : _exceptionMessages)
		{
			ImGui::TextColored(ImVec4{ 1.0f, .0f, .0f, 1.0f }, exceptionMessage.c_str());
		}

	}
}
