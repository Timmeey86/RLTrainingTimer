#pragma once

#include <optional>
#include <string>
#include <chrono>

namespace Core::Configuration::Commands
{
	struct AddTrainingProgramCommand
	{
	};
	struct RemoveTrainingProgramCommand
	{
	public:
		uint64_t TrainingProgramId;
	};
	struct RenameTrainingProgramCommand
	{
	public:
		uint64_t TrainingProgramId;
		std::string NewName;
	};
	struct SwapTrainingProgramCommand
	{
	public:
		uint64_t FirstTrainingProgramId;
		uint64_t SecondTrainingProgramId;
	};

	struct AddTrainingProgramEntryCommand
	{
	public:
		uint64_t TrainingProgramId;
	};
	struct RemoveTrainingProgramEntryCommand
	{
	public:
		uint64_t TrainingProgramId;
		uint16_t Position;
	};
	struct ChangeTrainingProgramEntryNameCommand
	{
	public:
		uint64_t TrainingProgramId;
		uint16_t Position;
		std::string NewName;
	};
	struct ChangeTrainingProgramEntryDurationCommand
	{
	public:
		uint64_t TrainingProgramId;
		uint16_t Position;
		std::chrono::milliseconds Duration;
	};
	struct SwapTrainingProgramEntriesCommand
	{
	public:
		uint64_t TrainingProgramId;
		uint16_t FirstPosition;
		uint16_t SecondPosition;
	};
}