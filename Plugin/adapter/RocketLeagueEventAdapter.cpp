#include <pch.h>
//
//#include "RocketLeagueEventAdapter.h"
//
//#include <chrono>
//
//using namespace std::chrono;
//
//namespace Adapter
//{
//	RocketLeagueEventAdapter::RocketLeagueEventAdapter(std::shared_ptr<Core::Training::Application::TrainingApplicationService> appService)
//		: _appService{ appService }
//	{
//	}
//	void RocketLeagueEventAdapter::hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper)
//	{
//		gameWrapper->HookEventPost("Function Engine.WorldInfo.EventPauseChanged", [gameWrapper, this](const std::string&) {
//			if (gameWrapper->IsPaused())
//			{
//				_appService->setGameState(Core::Training::Application::GameState::Paused);
//			}
//			else
//			{
//				_appService->setGameState(Core::Training::Application::GameState::NotPaused);
//			}
//		});
//
//		gameWrapper->HookEvent("Function TAGame.Replay_TA.Tick", [this](const std::string&) {
//			_appService->processTimerTick();
//		});
//	}
//}