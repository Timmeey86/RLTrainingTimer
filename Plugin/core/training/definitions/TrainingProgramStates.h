#pragma once

namespace Core::Training::Definitions
{
	/** Defines the states a training program can be in. */
	enum class TrainingProgramState
	{
		Uninitialized, // No training program is selected
		WaitingForStart, // A training program has been selected and is waiting to be started
		Running, // A training program has been started after being selected. Neither the game nor the training program are paused.
		Paused, // A training program has been paused after it was started, or the game has been paused (or both). Resuming will go back to "Running"
	};

	/** Defines the state of something which can be paused (game and training program). */
	enum class PausedState
	{
		NotPaused, // The game is not paused
		Paused, // The game is paused
	};
}
