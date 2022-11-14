#pragma once

#include <Plugin/configuration/data/TrainingProgramData.h>
#include <gmock/gmock.h>

class ITrainingProgramRepositoryMock : public configuration::ITrainingProgramRepository
{
	MOCK_METHOD(void, storeData, (const configuration::TrainingProgramListData& data), (override));
	MOCK_METHOD(configuration::TrainingProgramListData, restoreData, (), (const override));
};