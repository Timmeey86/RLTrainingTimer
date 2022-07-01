#pragma once

#include <gtest/gtest.h>
#include "../fakes/FakeTimeProvider.h"
#include "../mocks/IGameWrapperMock.h"
#include <Plugin/training/control/TrainingProgramFlowControl.h>

class TrainingProgramFlowTestFixture : public testing::Test
{
public:
	void SetUp() override
	{
		_gameWrapperMock = std::make_shared<::testing::StrictMock<IGameWrapperMock>>();
		_fakeTimeProvider = std::make_shared<FakeTimeProvider>();
		sut = std::make_unique<training::TrainingProgramFlowControl>(_gameWrapperMock, _fakeTimeProvider);

		IncompleteFreeplayEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteFreeplayEntry.Type = configuration::TrainingProgramEntryType::Freeplay;
		// No duration

		IncompleteCustomTrainingEntry.TimeMode = configuration::TrainingProgramCompletionMode::CompletePack;
		IncompleteCustomTrainingEntry.Type = configuration::TrainingProgramEntryType::CustomTraining;
		// No training pack code

		IncompleteDefaultEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteDefaultEntry.Type = configuration::TrainingProgramEntryType::Unspecified;
		// No duration

		IncompleteWorkshopEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		IncompleteWorkshopEntry.Duration = IncompleteWorkshopDuration;
		IncompleteWorkshopEntry.Type = configuration::TrainingProgramEntryType::WorkshopMap;
		// No workshop map path

		OneMinuteFreeplayEntry.Type = configuration::TrainingProgramEntryType::Freeplay;
		OneMinuteFreeplayEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		OneMinuteFreeplayEntry.Duration = std::chrono::milliseconds(60 * 1000);
		OneMinuteFreeplayEntry.Name = "OneMinuteFreeplayEntry";

		PackCompletionEntry.Type = configuration::TrainingProgramEntryType::CustomTraining;
		PackCompletionEntry.TimeMode = configuration::TrainingProgramCompletionMode::CompletePack;
		PackCompletionEntry.TrainingPackCode = DummyTrainingPackCode;
		PackCompletionEntry.Name = "PackCompletionEntry";

		OneMinuteWorkshopEntry.Type = configuration::TrainingProgramEntryType::WorkshopMap;
		OneMinuteWorkshopEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		OneMinuteWorkshopEntry.WorkshopMapPath = DummyWorkshopSubPath;
		OneMinuteWorkshopEntry.Duration = std::chrono::milliseconds(60 * 1000);
		OneMinuteWorkshopEntry.Name = "OneMinuteWorkshopEntry";

		TwoMinuteDefaultEntry.Type = configuration::TrainingProgramEntryType::Unspecified;
		TwoMinuteDefaultEntry.TimeMode = configuration::TrainingProgramCompletionMode::Timed;
		TwoMinuteDefaultEntry.Duration = std::chrono::milliseconds(2 * 60 * 1000);
		TwoMinuteDefaultEntry.Name = "TwoMinuteDefaultEntry";
	};
protected:
	std::unique_ptr<training::TrainingProgramFlowControl> sut;
	std::shared_ptr<IGameWrapperMock> _gameWrapperMock;
	std::shared_ptr<FakeTimeProvider> _fakeTimeProvider;

	configuration::TrainingProgramEntry EmptyEntry;
	configuration::TrainingProgramEntry IncompleteFreeplayEntry;
	configuration::TrainingProgramEntry IncompleteCustomTrainingEntry;
	configuration::TrainingProgramEntry IncompleteDefaultEntry;
	configuration::TrainingProgramEntry IncompleteWorkshopEntry;
	const std::chrono::milliseconds IncompleteWorkshopDuration = std::chrono::milliseconds(2000);
	configuration::TrainingProgramEntry OneMinuteFreeplayEntry;
	configuration::TrainingProgramEntry PackCompletionEntry;
	const std::string DummyTrainingPackCode = "ABC123";
	configuration::TrainingProgramEntry OneMinuteWorkshopEntry;
	const std::string DummyWorkshopSubPath = "FancyMap\\FancyMap.upk";
	configuration::TrainingProgramEntry TwoMinuteDefaultEntry;
};