#pragma once

#include <vector>
#include <string>

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
		bool SwitchingIsPossible;
		bool StartingIsPossible;
		bool PausingIsPossible;
		bool ResumingIsPossible;
		bool StoppingIsPossible;
	};
}
