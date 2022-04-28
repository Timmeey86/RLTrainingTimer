#pragma once

#include <vector>
#include <string>
#include <optional>

namespace training
{
	/** POD struct which combines an ID and a string. */
	struct IdAndTitle
	{
	public:
		uint64_t Id;
		std::string Title;
	};
	
	/** POD struct for data relevant for the UI which allows selecting, running, pausing etc of training programs. */
	struct TrainingProgramFlowData
	{
	public:
		std::vector<IdAndTitle> TrainingPrograms;
		std::optional<uint16_t> SelectedTrainingProgramIndex = {};
		bool SwitchingIsPossible;
		bool StartingIsPossible;
		bool PausingIsPossible;
		bool ResumingIsPossible;
		bool StoppingIsPossible;
	};
}
