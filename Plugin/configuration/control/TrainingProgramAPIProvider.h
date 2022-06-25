#pragma once

#include <API/RLTrainingTimerAPI.h>

/** This class implements the public API so other plugins can load training programs. */
class TrainingProgramAPIProvider : public RLTrainingTimerAPI
{
	// Inherited via RLTrainingTimerAPI
	void test() override;
	void injectTrainingSchedule(const std::string& uuid, const std::string& jsonData) override;
};