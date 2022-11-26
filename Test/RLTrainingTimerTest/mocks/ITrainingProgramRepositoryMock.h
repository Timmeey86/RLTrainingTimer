#pragma once

#include <Plugin/configuration/data/TrainingProgramData.h>
#include <gmock/gmock.h>

class ITrainingProgramRepositoryMock : public configuration::ITrainingProgramRepository
{
	MOCK_METHOD(void, storeData, (const configuration::TrainingProgramListData& data), (override));
	MOCK_METHOD(void, storeData, (const configuration::TrainingProgramListData& data, const std::string& location), (override));
	MOCK_METHOD(configuration::TrainingProgramListData, restoreData, (), (const override));
	MOCK_METHOD(configuration::TrainingProgramListData, restoreData, (const std::string& location), (const override));
	MOCK_METHOD(void, exportSingleTrainingProgram, (const configuration::TrainingProgramData& data, const std::string& location), (override));
	MOCK_METHOD(configuration::TrainingProgramData, importSingleTrainingProgram, (const std::string& location), (const override));
};
