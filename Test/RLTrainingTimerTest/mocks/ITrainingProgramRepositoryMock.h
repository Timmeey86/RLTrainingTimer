#pragma once

#include <Plugin/configuration/data/TrainingProgramData.h>
#include <gmock/gmock.h>

class ITrainingProgramRepositoryMock : public configuration::ITrainingProgramRepository
{
	MOCK_METHOD(void, storeData, (const configuration::TrainingProgramListData& data), (override));
	MOCK_METHOD(void, storeData, (const configuration::TrainingProgramListData& data, const std::filesystem::path &path), (override));
	MOCK_METHOD(configuration::TrainingProgramListData, restoreData, (), (const override));
	MOCK_METHOD(configuration::TrainingProgramListData, restoreData, (const std::filesystem::path &path), (const override));
};