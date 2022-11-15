#include <pch.h>
#include "TrainingProgramFlowControlUi.h"

#include <external/IMGUI/imgui.h>
#include <external/IMGUI/imgui_disable.h>

namespace training
{
	void TrainingProgramFlowControlUi::initTrainingProgramFlowControlUi(
		std::shared_ptr<GameWrapper> gameWrapper,
		std::shared_ptr<TrainingProgramFlowControl> flowControl)
	{
		_flowControl = flowControl;
		gameWrapper->RegisterDrawable([this, gameWrapper](const CanvasWrapper& canvasWrapper) {
			_trainingProgramDisplay->renderOneFrame(gameWrapper, canvasWrapper, _flowControl->getCurrentExecutionData());
		});
	}
	void TrainingProgramFlowControlUi::Render()
	{
		if (!_isWindowOpen)
		{
			toggleMenu();
			return;
		}

		if (_flowControl == nullptr)
		{
			return;
		}

		// Init GUI
		ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(FLT_MAX, FLT_MAX));
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_MenuBar;
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
		// TODO: An optimization would be to cache this and only update it whenever the list of training programs change
		auto flowData = _flowControl->getCurrentFlowData();

		static const char* selectedTrainingProgramName;
		if (flowData.SelectedTrainingProgramIndex.has_value())
		{
			selectedTrainingProgramName = flowData.TrainingPrograms.at(flowData.SelectedTrainingProgramIndex.value()).Title.c_str();
		}
		else
		{
			selectedTrainingProgramName = "Choose a program";
		}

		if (ImGui::BeginCombo("Training Program", selectedTrainingProgramName))
		{
			ImGui::Disable disable_selection_if_necessary(!flowData.SwitchingIsPossible);
			for (const auto& idAndTitle : flowData.TrainingPrograms)
			{
				const auto isSelected = flowData.SelectedTrainingProgramIndex.has_value() && flowData.TrainingPrograms.at(flowData.SelectedTrainingProgramIndex.value()).Id == idAndTitle.Id;
				if (ImGui::Selectable(idAndTitle.Title.c_str(), isSelected))
				{
					try
					{
						_flowControl->selectTrainingProgram(idAndTitle.Id);
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
			ImGui::Disable disable_start_if_necessary(!flowData.StartingIsPossible);
			if (ImGui::Button("Start") && flowData.StartingIsPossible)
			{
				_exceptionMessages.clear();
				try
				{
					_flowControl->startSelectedTrainingProgram();
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_pause_if_necessary(!flowData.PausingIsPossible);
			if (ImGui::Button("Pause") && flowData.PausingIsPossible)
			{
				try
				{
					_flowControl->pauseTrainingProgram();
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_resume_if_necessary(!flowData.ResumingIsPossible);
			if (ImGui::Button("Resume") && flowData.ResumingIsPossible)
			{
				try
				{
					_flowControl->resumeTrainingProgram();
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}
		ImGui::SameLine();
		{
			ImGui::Disable disable_stop_if_necessary(!flowData.StoppingIsPossible);
			if (ImGui::Button("Stop") && flowData.StoppingIsPossible)
			{
				try
				{
					_flowControl->stopRunningTrainingProgram();
				}
				catch (const std::runtime_error& ex)
				{
					_exceptionMessages.emplace_back(ex.what());
				}
			}
		}

		ImGui::SameLine();
		{
			ImGui::Disable disable_stop_if_necessary(!flowData.SkippingIsPossible);
			if (ImGui::Button("Skip") && flowData.SkippingIsPossible)
			{
				try
				{
					_flowControl->activateNextTrainingProgramStep();
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
			ImGui::TextColored(ImVec4{ 0.8f, .1f, .1f, 1.0f }, exceptionMessage.c_str());
		}

	}

	void TrainingProgramFlowControlUi::displayErrorMessage(const std::string& shortText, const std::string& errorDescription)
	{
		if (!_isWindowOpen)
		{
			toggleMenu();
		}
		_exceptionMessages.emplace_back(fmt::format("{}: {}", shortText, errorDescription));
		LOG("Error occured: {} ({})", shortText, errorDescription);
	}

	void TrainingProgramFlowControlUi::clearErrorMessages()
	{
		_exceptionMessages.clear();
	}
}
