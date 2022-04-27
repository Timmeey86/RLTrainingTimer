#pragma once

#include "../data/TrainingProgramData.h"

namespace configuration
{
	/** Simple interface for classes which consume training programs. */
	class ITrainingProgramListReceiver
	{
	protected:
		ITrainingProgramListReceiver() = default;

	public:
		virtual ~ITrainingProgramListReceiver() = default;

		/** Populates the receiver with the given list of training programs. */
		virtual void receiveListData(const TrainingProgramListData& data) = 0;
	};
}