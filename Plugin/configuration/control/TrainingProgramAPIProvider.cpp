#include "pch.h"
#include "TrainingProgramAPIProvider.h"

void TrainingProgramAPIProvider::test()
{
	LOG("Test Successful");
}

void TrainingProgramAPIProvider::injectTrainingSchedule(const std::string& uuid, const std::string& jsonData)
{
	// TODO:
	// - Check if uuid is known
	// - Create new program if uuid is not known
	// - Clear program if uuid is known
	// - Deserialize json data (with exception handling!)
	// - Replace ID in json data by next free local ID
	// - Fill the created/existing program with deserialized data
}
