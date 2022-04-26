//#include <gmock/gmock.h>
//
//#include <Plugin/core/configuration/domain/TrainingProgram.h>
//#include <Plugin/core/kernel/IndexOutOfBoundsException.h>
//#include <Plugin/core/configuration/events/TrainingProgramEntryEvents.h>
//#include <Plugin/core/configuration/events/TrainingProgramEvents.h>
//
//namespace Core::Configuration::Test
//{
//	class TrainingProgramTestFixture : public testing::Test
//	{
//	public:
//		void SetUp() override
//		{
//			sut = std::make_unique<Domain::TrainingProgram>(DefaultId);
//			sut->addEntry({ FirstEntryName, FirstEntryDuration });
//			sut->addEntry({ SecondEntryName, SecondEntryDuration });
//		}
//	protected:
//		static const uint64_t DefaultId = 0ULL;
//
//		static const std::string FirstEntryName;
//		static const std::string SecondEntryName;
//
//		static const std::chrono::milliseconds FirstEntryDuration;
//		static const std::chrono::milliseconds SecondEntryDuration;
//
//		static const std::chrono::milliseconds InitialDuration;
//		static const size_t InitialSize = 2;
//
//		std::unique_ptr<Domain::TrainingProgram> sut;
//	};
//	const std::string TrainingProgramTestFixture::FirstEntryName = "First Entry";
//	const std::string TrainingProgramTestFixture::SecondEntryName = "Second Entry";
//	const std::chrono::milliseconds TrainingProgramTestFixture::FirstEntryDuration = std::chrono::minutes(4);
//	const std::chrono::milliseconds TrainingProgramTestFixture::SecondEntryDuration = std::chrono::minutes(6);
//	const std::chrono::milliseconds TrainingProgramTestFixture::InitialDuration = FirstEntryDuration + SecondEntryDuration;
//
//	TEST_F(TrainingProgramTestFixture, verify_fixture)
//	{
//		ASSERT_NE(sut, nullptr);
//
//		EXPECT_EQ(sut->id(), DefaultId);
//		EXPECT_EQ(sut->programDuration(), InitialDuration);
//		auto entries = sut->entries();
//		ASSERT_EQ(entries.size(), InitialSize);
//		EXPECT_EQ(entries[0].Name, FirstEntryName);
//		EXPECT_EQ(entries[0].Duration, FirstEntryDuration);
//		EXPECT_EQ(entries[1].Name, SecondEntryName);
//		EXPECT_EQ(entries[1].Duration, SecondEntryDuration);
//	}
//
//	// Tests that an entry will be added properly, and that a proper event is being created
//	TEST_F(TrainingProgramTestFixture, addEntry_when_calledOnce_then_producesOneEntry)
//	{
//		auto entryName = "New Entry";
//		auto entryDuration = std::chrono::seconds(2);
//		auto genericEvents = sut->addEntry({ entryName, entryDuration });
//		
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto addEvent = dynamic_cast<Events::TrainingProgramEntryAddedEvent*>(genericEvents.at(0).get());
//		auto changeEvent = dynamic_cast<Events::TrainingProgramChangedEvent*>(genericEvents.at(1).get());
//		ASSERT_NE(addEvent, nullptr);
//		ASSERT_NE(changeEvent, nullptr);
//
//		EXPECT_EQ(sut->programDuration(), InitialDuration + entryDuration);
//		EXPECT_EQ(addEvent->TrainingProgramId, DefaultId);
//		EXPECT_EQ(addEvent->TrainingProgramEntryDuration, entryDuration);
//		EXPECT_EQ(addEvent->TrainingProgramEntryName, entryName);
//		ASSERT_EQ(sut->entries().size(), InitialSize + 1);
//		EXPECT_EQ(sut->entries().back().Duration, addEvent->TrainingProgramEntryDuration);
//		EXPECT_EQ(sut->entries().back().Name, addEvent->TrainingProgramEntryName);
//	}
//
//	// Tests that the same entry can be added twice (the user might want to do entry 1, then 2, then 1 again, then 3, for example).
//	TEST_F(TrainingProgramTestFixture, addEntry_when_calledTwice_then_producesTwoEntries)
//	{
//		auto entryName = "New Entry";
//		auto entryDuration = std::chrono::seconds(2);
//		sut->addEntry({ entryName, entryDuration });
//		sut->addEntry({ entryName, entryDuration });
//
//		EXPECT_EQ(sut->programDuration(), InitialDuration + 2 * entryDuration);
//
//		auto entries = sut->entries();
//		ASSERT_EQ(entries.size(), InitialSize + 2);
//
//		const auto& firstEntry = entries[entries.size() - 2];
//		const auto& secondEntry = entries[entries.size() - 1];
//		EXPECT_EQ(firstEntry.Name, secondEntry.Name);
//		EXPECT_EQ(firstEntry.Duration, secondEntry.Duration);
//	}
//
//	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithInvalidPosition_will_throw)
//	{
//		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"position",			// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			-1);				// Value which will be supplied by this test
//		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"position",			// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			InitialSize);		// Value which will be supplied by this test
//
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->removeEntry(-1);
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), firstExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->removeEntry(InitialSize);
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), secondExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//
//	}
//
//	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithFirstPos_then_removesFirstElement)
//	{
//		ASSERT_GE(sut->entries().size(), 2);
//
//		auto genericEvents = sut->removeEntry(0);
//
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvents.at(0).get());
//		auto changeEvent = dynamic_cast<Events::TrainingProgramChangedEvent*>(genericEvents.at(1).get());
//		ASSERT_NE(removalEvent, nullptr);
//		ASSERT_NE(changeEvent, nullptr);
//
//		EXPECT_EQ(removalEvent->TrainingProgramId, DefaultId);
//		EXPECT_EQ(removalEvent->TrainingProgramEntryPosition, 0);
//
//		EXPECT_EQ(sut->entries().size(), 1);
//		EXPECT_EQ(sut->programDuration(), SecondEntryDuration);
//	}
//	TEST_F(TrainingProgramTestFixture, removeEntry_when_calledWithLastPos_then_removesLastElement)
//	{
//		ASSERT_GE(sut->entries().size(), 2);
//
//		auto genericEvents = sut->removeEntry(InitialSize - 1);
//
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvents.at(0).get());
//		auto changeEvent = dynamic_cast<Events::TrainingProgramChangedEvent*>(genericEvents.at(1).get());
//		ASSERT_NE(removalEvent, nullptr);
//		ASSERT_NE(changeEvent, nullptr);
//
//		EXPECT_EQ(removalEvent->TrainingProgramId, DefaultId);
//		EXPECT_EQ(removalEvent->TrainingProgramEntryPosition, InitialSize - 1);
//
//		EXPECT_EQ(sut->entries().size(), 1);
//		EXPECT_EQ(sut->programDuration(), FirstEntryDuration);
//	}
//
//	TEST_F(TrainingProgramTestFixture, renameEntry_when_calledWithInvalidPosition_will_throw)
//	{
//		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"position",			// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			-1);				// Value which will be supplied by this test
//		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"position",			// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			InitialSize);		// Value which will be supplied by this test
//
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->renameEntry(-1, "");
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), firstExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->renameEntry(InitialSize, "");
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), secondExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//
//	}
//
//	TEST_F(TrainingProgramTestFixture, renameEntry_when_calledWithFirstPos_then_replacesFirstEntry)
//	{
//		ASSERT_GE(sut->entries().size(), 1);
//
//		auto newName = "New entry name";
//
//		auto genericEvents = sut->renameEntry(0, newName);
//
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto renameEvent = dynamic_cast<Events::TrainingProgramEntryRenamedEvent*>(genericEvents.at(0).get());
//		auto changeEvent = dynamic_cast<Events::TrainingProgramChangedEvent*>(genericEvents.at(1).get());
//		ASSERT_NE(renameEvent, nullptr);
//		ASSERT_NE(changeEvent, nullptr);
//
//		auto sutEntries = sut->entries();
//		EXPECT_EQ(sutEntries.size(), InitialSize);
//		
//		EXPECT_EQ(renameEvent->TrainingProgramId, DefaultId);
//		EXPECT_EQ(renameEvent->TrainingProgramEntryName, newName);
//		EXPECT_EQ(renameEvent->TrainingProgramEntryPosition, 0);
//	}
//
//	TEST_F(TrainingProgramTestFixture, swapEntries_when_calledWithInvalidPosition_will_throw)
//	{
//		auto firstExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"first position",	// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			-1);				// Value which will be supplied by this test
//		auto secondExpectedException = Kernel::IndexOutOfBoundsException(
//			"Configuration",	// name of the bounded context
//			"Aggregate",		// DDD type of the throwing class
//			"TrainingProgram",	// Name of the throwing class
//			"second position",	// Name of the violating argument
//			0,					// Possible minimum value
//			InitialSize - 1,	// Possible maximum value
//			InitialSize);		// Value which will be supplied by this test
//
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->swapEntries(-1, 0);
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), firstExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//		EXPECT_THROW(
//			{
//				try
//				{
//					sut->swapEntries(0, InitialSize);
//				}
//				catch (Kernel::IndexOutOfBoundsException& ex)
//				{
//					EXPECT_STREQ(ex.what(), secondExpectedException.what());
//					throw;
//				}
//			}, Kernel::IndexOutOfBoundsException
//		);
//	}
//
//	TEST_F(TrainingProgramTestFixture, swapEntries_when_calledWithFirstTwoPositions_will_swapFirstTwoElements)
//	{
//		ASSERT_GE(sut->entries().size(), 2);
//
//		auto genericEvents = sut->swapEntries(0, 1);
//
//		ASSERT_EQ(genericEvents.size(), 2);
//
//		auto swapEvent = dynamic_cast<Events::TrainingProgramEntrySwappedEvent*>(genericEvents.at(0).get());
//		auto changeEvent = dynamic_cast<Events::TrainingProgramChangedEvent*>(genericEvents.at(1).get());
//		ASSERT_NE(swapEvent, nullptr);
//		ASSERT_NE(changeEvent, nullptr);
//
//		auto sutEntries = sut->entries();
//		EXPECT_EQ(sut->programDuration(), InitialDuration);
//		ASSERT_EQ(sutEntries.size(), InitialSize);
//		EXPECT_EQ(sutEntries[0].Name, SecondEntryName);
//		EXPECT_EQ(sutEntries[0].Duration, SecondEntryDuration);
//		EXPECT_EQ(sutEntries[1].Name, FirstEntryName);
//		EXPECT_EQ(sutEntries[1].Duration, FirstEntryDuration);
//
//		EXPECT_EQ(swapEvent->TrainingProgramId, DefaultId);
//		EXPECT_EQ(swapEvent->FirstTrainingProgramEntryPosition, 0);
//		EXPECT_EQ(swapEvent->SecondTrainingProgramEntryPosition, 1);
//	}
//}