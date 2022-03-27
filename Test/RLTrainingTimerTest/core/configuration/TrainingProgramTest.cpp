#include <gmock/gmock.h>

#include <Plugin/core/configuration/domain/TrainingProgram.h>
#include <Plugin/core/kernel/IndexOutOfBoundsException.h>
#include <Plugin/core/configuration/events/TrainingProgramEntryEvents.h>
#include <Plugin/core/configuration/events/TrainingProgramEvents.h>

namespace Core::Configuration::Test
{
	class TrainingProgramTestFixture : public testing::Test
	{
	public:
		virtual void SetUp() override
		{
			sut = std::make_unique<Domain::TrainingProgram>(DefaultId);
			sut->addEntry({ FirstEntryName, FirstEntryDuration });
			sut->addEntry({ SecondEntryName, SecondEntryDuration });
		}
	protected:
		static const uint64_t DefaultId = 0ULL;

		static const std::string FirstEntryName;
		static const std::string SecondEntryName;

		static const uint32_t FirstEntryDuration = 4000;
		static const uint32_t SecondEntryDuration = 6000;

		static const uint32_t InitialDuration = FirstEntryDuration + SecondEntryDuration;
		static const size_t InitialSize = 2;

		std::unique_ptr<Domain::TrainingProgram> sut;
	};
	const std::string TrainingProgramTestFixture::FirstEntryName = "First Entry";
	const std::string TrainingProgramTestFixture::SecondEntryName = "Second Entry";

	TEST_F(TrainingProgramTestFixture, verify_fixture)
	{
		EXPECT_EQ(sut->id(), DefaultId);
		EXPECT_EQ(sut->programDuration(), InitialDuration);
		auto entries = sut->entries();
		ASSERT_EQ(entries.size(), InitialSize);
		EXPECT_EQ(entries[0].name(), FirstEntryName);
		EXPECT_EQ(entries[0].duration(), FirstEntryDuration);
		EXPECT_EQ(entries[1].name(), SecondEntryName);
		EXPECT_EQ(entries[1].duration(), SecondEntryDuration);
	}

	// Tests that an entry will be added properly, and that a proper event is being created
	TEST_F(TrainingProgramTestFixture, addEntry_when_calledOnce_then_producesOneEntry)
	{
		auto entryName = "New Entry";
		auto entryDuration = 2000U;
		auto genericResult = sut->addEntry({ entryName, entryDuration });
		
		auto specificEvent = dynamic_cast<Events::TrainingProgramEntryAddedEvent*>(genericResult.get());
		ASSERT_NE(specificEvent, nullptr);

		EXPECT_EQ(sut->programDuration(), InitialDuration + entryDuration);
		EXPECT_EQ(specificEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(specificEvent->TrainingProgramEntryDuration, entryDuration);
		EXPECT_EQ(specificEvent->TrainingProgramEntryName, entryName);
		ASSERT_EQ(sut->entries().size(), InitialSize + 1);
		EXPECT_EQ(sut->entries().back().duration(), specificEvent->TrainingProgramEntryDuration);
		EXPECT_EQ(sut->entries().back().name(), specificEvent->TrainingProgramEntryName);
	}

	// Tests that the same entry can be added twice (the user might want to do entry 1, then 2, then 1 again, then 3, for example).
	TEST_F(TrainingProgramTestFixture, addEntry_when_calledTwice_then_producesTwoEntries)
	{
		auto entryName = "New Entry";
		auto entryDuration = 2000U;
		sut->addEntry({ entryName, entryDuration });
		sut->addEntry({ entryName, entryDuration });

		EXPECT_EQ(sut->programDuration(), InitialDuration + 2 * entryDuration);

		auto entries = sut->entries();
		ASSERT_EQ(entries.size(), InitialSize + 2);

		const auto& firstEntry = entries[entries.size() - 2];
		const auto& secondEntry = entries[entries.size() - 1];
		EXPECT_EQ(firstEntry.name(), secondEntry.name());
		EXPECT_EQ(firstEntry.duration(), secondEntry.duration());
	}

	// Tests that replaying an addition event will produce the same object as the initial addition
	TEST_F(TrainingProgramTestFixture, addEntry_when_restored_then_producesSameResult)
	{
		// Add an entry to an empty training program and remember the event
		Domain::TrainingProgram manuallyCreatedProgram(DefaultId);
		auto entryName = "New Entry";
		auto entryDuration = 2000U;
		auto additionEvent = manuallyCreatedProgram.addEntry({ entryName, entryDuration });

		// Create another empty training program, but this time restore it through events
		Domain::TrainingProgram restoredProgram(DefaultId);
		restoredProgram.applyEvents({ additionEvent });

		auto manualEntries = manuallyCreatedProgram.entries();
		auto restoredEntries = restoredProgram.entries();

		EXPECT_EQ(manualEntries.size(), restoredEntries.size());
		ASSERT_EQ(restoredEntries.size(), 1);
		EXPECT_EQ(manuallyCreatedProgram.programDuration(), restoredProgram.programDuration());
		EXPECT_EQ(manualEntries.back().name(), restoredEntries.back().name());
		EXPECT_EQ(manualEntries.back().duration(), restoredEntries.back().duration());
	}

	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithInvalidPosition_will_throw)
	{
		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"position",			// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			-1);				// Value which will be supplied by this test
		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"position",			// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			InitialSize);		// Value which will be supplied by this test

		EXPECT_THROW(
			{
				try
				{
					sut->removeEntry(-1);
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), firstExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);
		EXPECT_THROW(
			{
				try
				{
					sut->removeEntry(InitialSize);
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), secondExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);

	}

	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithFirstPos_then_removesFirstElement)
	{
		ASSERT_GE(sut->entries().size(), 2);

		auto genericEvent = sut->removeEntry(0);
		auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvent.get());
		ASSERT_NE(removalEvent, nullptr);

		EXPECT_EQ(removalEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(removalEvent->TrainingProgramEntryPosition, 0);

		EXPECT_EQ(sut->entries().size(), 1);
		EXPECT_EQ(sut->programDuration(), SecondEntryDuration);
	}
	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithLastPos_then_removesLastElement)
	{
		ASSERT_GE(sut->entries().size(), 2);

		auto genericEvent = sut->removeEntry(InitialSize - 1);
		auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvent.get());
		ASSERT_NE(removalEvent, nullptr);

		EXPECT_EQ(removalEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(removalEvent->TrainingProgramEntryPosition, InitialSize - 1);

		EXPECT_EQ(sut->entries().size(), 1);
		EXPECT_EQ(sut->programDuration(), FirstEntryDuration);
	}
	TEST_F(TrainingProgramTestFixture, removeEntry_when_restored_then_producesSameResult)
	{
		ASSERT_GE(sut->entries().size(), 2);

		auto restoredTrainingProgram = Domain::TrainingProgram(*sut);

		auto removalEvent = sut->removeEntry(0);

		restoredTrainingProgram.applyEvents({ removalEvent });

		auto sutEntries = sut->entries();
		auto restoredEntries = restoredTrainingProgram.entries();

		ASSERT_GE(sutEntries.size(), 1);
		ASSERT_GE(restoredEntries.size(), 1);

		EXPECT_EQ(sutEntries.size(), restoredEntries.size());
		EXPECT_EQ(sut->programDuration(), restoredTrainingProgram.programDuration());
	}

	TEST_F(TrainingProgramTestFixture, replaceEntry_when_calledWithInvalidPosition_will_throw)
	{
		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"position",			// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			-1);				// Value which will be supplied by this test
		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"position",			// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			InitialSize);		// Value which will be supplied by this test

		EXPECT_THROW(
			{
				try
				{
					sut->replaceEntry(-1, { "", 0 });
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), firstExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);
		EXPECT_THROW(
			{
				try
				{
					sut->replaceEntry(InitialSize, { "", 0 });
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), secondExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);

	}

	TEST_F(TrainingProgramTestFixture, replaceEntry_when_calledWithFirstPos_then_replacesFirstEntry)
	{
		ASSERT_GE(sut->entries().size(), 1);

		auto newName = "New entry name";
		auto newDuration = 15000U;

		auto genericEvent = sut->replaceEntry(0, { newName, newDuration });
		auto replacementEvent = dynamic_cast<Events::TrainingProgramEntryUpdatedEvent*>(genericEvent.get());
		ASSERT_NE(replacementEvent, nullptr);

		auto sutEntries = sut->entries();
		EXPECT_EQ(sutEntries.size(), InitialSize);
		EXPECT_EQ(sut->programDuration(), InitialDuration - FirstEntryDuration + newDuration);
		
		EXPECT_EQ(replacementEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(replacementEvent->TrainingProgramEntryDuration, newDuration);
		EXPECT_EQ(replacementEvent->TrainingProgramEntryName, newName);
		EXPECT_EQ(replacementEvent->TrainingProgramEntryPosition, 0);
	}

	TEST_F(TrainingProgramTestFixture, replaceEntry_when_restored_then_producesSameResult)
	{
		ASSERT_GE(sut->entries().size(), 1);

		auto newName = "New entry name";
		auto newDuration = 15000U;

		auto restoredTrainingProgram = Domain::TrainingProgram(*sut);

		auto replacementEvent = sut->replaceEntry(0, { newName, newDuration });

		restoredTrainingProgram.applyEvents({ replacementEvent });

		auto sutEntries = sut->entries();
		auto restoredEntries = restoredTrainingProgram.entries();

		ASSERT_GE(sutEntries.size(), InitialSize);
		ASSERT_GE(restoredEntries.size(), InitialSize);

		EXPECT_EQ(sutEntries.size(), restoredEntries.size());
		EXPECT_EQ(sut->programDuration(), restoredTrainingProgram.programDuration());
	}

	TEST_F(TrainingProgramTestFixture, swapEntries_when_calledWithInvalidPosition_will_throw)
	{
		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"first position",	// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			-1);				// Value which will be supplied by this test
		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
			"Configuration",	// name of the bounded context
			"Aggregate",		// DDD type of the throwing class
			"TrainingProgram",	// Name of the throwing class
			"second position",	// Name of the violating argument
			0,					// Possible minimum value
			InitialSize - 1,	// Possible maximum value
			InitialSize);		// Value which will be supplied by this test

		EXPECT_THROW(
			{
				try
				{
					sut->swapEntries(-1, 0);
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), firstExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);
		EXPECT_THROW(
			{
				try
				{
					sut->swapEntries(0, InitialSize);
				}
				catch (Kernel::IndexOutOfBoundsException& ex)
				{
					EXPECT_STREQ(ex.what(), secondExpectedException.what());
					throw;
				}
			}, Kernel::IndexOutOfBoundsException
		);
	}

	TEST_F(TrainingProgramTestFixture, swapEntries_when_calledWithFirstTwoPositions_will_swapFirstTwoElements)
	{
		ASSERT_GE(sut->entries().size(), 2);

		auto genericEvent = sut->swapEntries(0, 1);
		auto swapEvent = dynamic_cast<Events::TrainingProgramEntrySwappedEvent*>(genericEvent.get());
		ASSERT_NE(swapEvent, nullptr);

		auto sutEntries = sut->entries();
		EXPECT_EQ(sut->programDuration(), InitialDuration);
		ASSERT_EQ(sutEntries.size(), InitialSize);
		EXPECT_EQ(sutEntries[0].name(), SecondEntryName);
		EXPECT_EQ(sutEntries[0].duration(), SecondEntryDuration);
		EXPECT_EQ(sutEntries[1].name(), FirstEntryName);
		EXPECT_EQ(sutEntries[1].duration(), FirstEntryDuration);

		EXPECT_EQ(swapEvent->TrainingProgramId, DefaultId);
		EXPECT_EQ(swapEvent->FirstTrainingProgramEntryPosition, 0);
		EXPECT_EQ(swapEvent->SecondTrainingProgramEntryPosition, 1);
	}

	TEST_F(TrainingProgramTestFixture, swapEntries_when_restored_then_producesSameResult)
	{
		ASSERT_GE(sut->entries().size(), 2);

		auto restoredProgram = Domain::TrainingProgram(*sut);

		auto swapEvent = sut->swapEntries(0, 1);

		restoredProgram.applyEvents({ swapEvent });

		auto sutEntries = sut->entries();
		auto restoredEntries = restoredProgram.entries();

		ASSERT_EQ(sutEntries.size(), restoredEntries.size());
		for (int index = 0; index < sutEntries.size(); index++)
		{
			EXPECT_EQ(sutEntries[index].name(), restoredEntries[index].name());
			EXPECT_EQ(sutEntries[index].duration(), restoredEntries[index].duration());
		}
		EXPECT_EQ(sut->programDuration(), restoredProgram.programDuration());
	}
}