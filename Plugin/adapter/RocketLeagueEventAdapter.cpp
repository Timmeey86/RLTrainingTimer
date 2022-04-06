#include <pch.h>

#include "RocketLeagueEventAdapter.h"

#include <chrono>

using namespace std::chrono;
using PausedState = Core::Training::Definitions::PausedState;

namespace Adapter
{
	RocketLeagueEventAdapter::RocketLeagueEventAdapter(std::shared_ptr<Core::Training::Application::TrainingApplicationService> appService)
		: _appService{ appService }
	{
	}
	void RocketLeagueEventAdapter::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper)
	{
		gameWrapper->HookEventPost("Function Engine.WorldInfo.EventPauseChanged", [gameWrapper, this](const std::string&) {
			if (gameWrapper->IsPaused())
			{
				_appService->setGameState(PausedState::Paused);
			}
			else
			{
				_appService->setGameState(PausedState::NotPaused);
			}
		});

		gameWrapper->HookEvent("Function TAGame.Replay_TA.Tick", [this](const std::string&) {
			_appService->processTimerTick();
		});
	}
}