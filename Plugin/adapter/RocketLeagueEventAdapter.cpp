#include <pch.h>

#include "RocketLeagueEventAdapter.h"

#include <chrono>

using namespace std::chrono;

namespace Adapter
{
	void RocketLeagueEventAdapter::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper)
	{
		static bool timerIsRunning = false;
		static std::chrono::steady_clock::time_point referenceTime;
		static std::chrono::steady_clock::time_point pauseStartTime;

		gameWrapper->HookEvent("Function Engine.WorldInfo.EventPauseChanged", [gameWrapper](const std::string&) {
			if (gameWrapper->IsPaused())
			{
				// Store the start of the pause time
				pauseStartTime = steady_clock::now();
			}
			else
			{
				// Create a new fake start time so we can act like the pause never existed, but the player started later instead
				auto pauseDifference = (pauseStartTime - referenceTime);

				LOG("Shifting start point by {} ms", duration_cast<milliseconds>(pauseDifference).count());
				referenceTime = steady_clock::now() - pauseDifference;
			}
		});
		_globalCvarManager->registerNotifier("tmp_start", [this](std::vector<std::string> args) {
			referenceTime = steady_clock::now();
			timerIsRunning = true;
		}, "", PERMISSION_ALL);
		_globalCvarManager->registerNotifier("tmp_stop", [this](std::vector<std::string> args) {
			timerIsRunning = false;
			LOG("Total duration: {} ms", duration_cast<milliseconds>(steady_clock::now() - referenceTime).count());
		}, "", PERMISSION_ALL);
		




		gameWrapper->HookEvent("Function TAGame.Replay_TA.Tick", [](const std::string&) {
			if (timerIsRunning)
			{
				//LOG("Tick");
			}
		});
	}
}