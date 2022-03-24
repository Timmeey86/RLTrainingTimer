#include <gmock/gmock.h>

#include <Plugin/core/configuration/domain/TrainingProgram.h>

namespace Core::Configuration::Test
{
	class TrainingProgramTestFixture : public testing::Test
	{
	protected:
		static const uint64_t DefaultId = 0ULL;
		static const std::string DefaultEntryName;
		static const uint32_t DefaultDuration = 5;
		std::unique_ptr<Domain::TrainingProgram> sut = std::make_unique<Domain::TrainingProgram>(DefaultId);
	};
	const std::string TrainingProgramTestFixture::DefaultEntryName = "Test entry";

	TEST_F(TrainingProgramTestFixture, verify_fixture)
	{
		EXPECT_EQ(sut->id(), DefaultId);
		EXPECT_EQ(sut->programDuration(), 0);
		EXPECT_EQ(sut->entries().size(), 0);
	}

	// Tests that an entry will be added properly, and that a proper event is being created
	TEST_F(TrainingProgramTestFixture, addEntry_when_calledOnce_then_producesOneEntry)
	{		
		auto result = sut->addEntry({ DefaultEntryName, DefaultDuration });

		EXPECT_EQ(sut->programDuration(), DefaultDuration);
		EXPECT_EQ(result->TrainingProgramId, DefaultId);
		EXPECT_EQ(result->TrainingProgramEntryDuration, DefaultDuration);
		EXPECT_EQ(result->TrainingProgramEntryName, DefaultEntryName);
		ASSERT_EQ(sut->entries().size(), 1);
		EXPECT_EQ(sut->entries().front().duration(), result->TrainingProgramEntryDuration);
		EXPECT_EQ(sut->entries().front().name(), result->TrainingProgramEntryName);
	}

	// Tests that the same entry can be added twice (the user might want to do entry 1, then 2, then 1 again, then 3, for example).
	TEST_F(TrainingProgramTestFixture, addEntry_when_calledTwice_then_producesTwoEntries)
	{
		sut->addEntry({ DefaultEntryName, DefaultDuration });
		sut->addEntry({ DefaultEntryName, DefaultDuration });

		EXPECT_EQ(sut->programDuration(), DefaultDuration * 2);

		auto entries = sut->entries();
		ASSERT_EQ(entries.size(), 2);

		auto& firstEntry = entries[0];
		auto& secondEntry = entries[1];
		EXPECT_EQ(firstEntry.name(), secondEntry.name());
		EXPECT_EQ(firstEntry.duration(), secondEntry.duration());
	}

	// Tests that replaying an addition event will produce the same object as the initial addition
	TEST_F(TrainingProgramTestFixture, addEntry_when_restored_then_producesSameResult)
	{
		auto additionEvent = sut->addEntry({ DefaultEntryName, DefaultDuration });

		Domain::TrainingProgram restoredProgram(additionEvent->TrainingProgramId);
		restoredProgram.loadFromEvents({ additionEvent });

		auto sutEntries = sut->entries();
		auto restoredEntries = restoredProgram.entries();

		EXPECT_EQ(sutEntries.size(), restoredEntries.size());
		ASSERT_EQ(restoredEntries.size(), 1);
		EXPECT_EQ(sut->programDuration(), restoredProgram.programDuration());
		EXPECT_EQ(sutEntries[0].name(), restoredEntries[0].name());
		EXPECT_EQ(sutEntries[0].duration(), restoredEntries[0].duration());
	}
}