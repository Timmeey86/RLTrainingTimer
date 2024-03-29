#pragma once

#include <vector>
#include <string>
#include <optional>

namespace training
{
	/** POD struct which combines a UUID and a string. */
	struct IdAndTitle
	{
	public:
		std::string Id;
		std::string Title;
	};
	
	/** POD struct for data relevant for the UI which allows selecting, running, pausing etc of training programs. */
	struct TrainingProgramFlowData
	{
	public:
		std::vector<IdAndTitle> TrainingPrograms;
		std::optional<uint16_t> SelectedTrainingProgramIndex = {};
		bool SwitchingIsPossible = true;
		bool StartingIsPossible = false;
		bool PausingIsPossible = false;
		bool ResumingIsPossible = false;
		bool StoppingIsPossible = false;
		bool SkippingIsPossible = false;
	};
}
