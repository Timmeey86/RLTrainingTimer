//#pragma once
//
//namespace Core::Training::Definitions
//{
//	/** Defines the states a training program can be in. */
//	enum class TrainingProgramState
//	{
//		Uninitialized, // No training program is selected
//		WaitingForStart, // A training program has been selected and is waiting to be started
//		Running, // A training program has been started after being selected. Neither the game nor the training program are paused.
//		OnlyProgramPaused, // The training program has been paused, but the game has not
//		OnlyGamePaused, // The training program is still "running", but the game is paused. The UI might still want to allow pausing the training program explicilty,
//						// so it does not continue once the game gets unpaused.
//		BothPaused, // Both the training program and the game are paused
//	};
//
//	/** Defines the state of something which can be paused (game and training program). */
//	enum class PausedState
//	{
//		NotPaused, // The game is not paused
//		Paused, // The game is paused
//	};
//}
