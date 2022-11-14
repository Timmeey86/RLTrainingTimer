#include <gmock/gmock.h>
#include <Plugin/configuration/control/TrainingProgramListConfigurationControl.h>
#include "../mocks/ITrainingProgramRepositoryMock.h"
#include <Plugin/external/fmt/include/fmt/format.h>
TEST(TEMP, ensureUniqueIds)
{
	std::set<std::string> knownUuids;

	for (auto i = 0; i < 100; i++)
	{
		auto trainingProgramDtaa = std::make_shared<std::map<std::string, configuration::TrainingProgramData>>();
		auto repository = std::make_shared<::testing::NiceMock<ITrainingProgramRepositoryMock>>();
		auto sut = std::make_unique<configuration::TrainingProgramListConfigurationControl>(trainingProgramDtaa, repository);

		for (auto j = 0; j < 100; j++)
		{
			auto uuid = sut->addTrainingProgram();
			EXPECT_EQ(knownUuids.count(uuid), 0) << std::string("UUID ") + uuid + " occured more than once. Set has " + std::to_string(knownUuids.size()) + " entries";
			knownUuids.emplace(uuid);
		}
	}
}