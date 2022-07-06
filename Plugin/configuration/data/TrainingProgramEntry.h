#pragma once

#include <string>
#include <chrono>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!!!! KEEP THIS FILE CONSISTENT WITH THE PREJUMP PLUGIN !!!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

namespace configuration
{
	/** This allows figuring out which type of training program entry an entry is. */
	enum class TrainingProgramEntryType
	{
		Unspecified, // The user just assigns a label and a duration and figures out what to do himself
		Freeplay, // The user wants to do this training step in free play, and automatically load into freeplay if he's not in there
		CustomTraining, // The user wants to specify a training pack code and load into that
		WorkshopMap, // Allows loading a workshop map from a given path
	};

	/** This allows switching between using a timeout, or the completion of a training pack as a trigger. */
	enum class TrainingProgramCompletionMode
	{
		Timed, // The entry will be executed for a fixed duration (can be paused)
		CompletePack, // The entry will be executed until the training end screen has been reached for the current pack.
	};

	/** Defines values for bakkesmod training variance options so they can be configured for each training program entry separately. 
	 *
	 * Set useDefaultSettings to false if you actually set values here.
	 */
	struct VarianceSettings
	{
	public:
		bool UseDefaultSettings = true;
		std::string EnableTraining;
		std::string LimitBoost;
		std::string AllowMirror;
		std::string PlayerVelocity;
		std::string VarSpeed;
		std::string VarLoc;
		std::string VarLocZ;
		std::string Shuffle;
		std::string VarCarLoc;
		std::string VarCarRot;
		std::string VarSpin;
		std::string VarRot;
	};

	/** Defines the contents of a single training "unit". */
	class TrainingProgramEntry
	{
	public:
		std::string Name;
		TrainingProgramEntryType Type = TrainingProgramEntryType::Unspecified;
		std::string TrainingPackCode; // Only set when Type = CustomTraining
		std::string WorkshopMapPath; // Only set when Type = WorkshopMap. Contains just the end of the path; the base path is configurable in the UI
		TrainingProgramCompletionMode TimeMode = TrainingProgramCompletionMode::Timed;
		std::chrono::milliseconds Duration; // 0 if CompletionMode != Timed
		std::string Notes; // Any kind of additional notes for this step.
		VarianceSettings Variance;
	};
}

