#pragma once

#include <bakkesmod/wrappers/GameWrapper.h>

#include <memory>
namespace Adapter
{
	/** This class adapts the events sent by Rocket League into domain events that we can more easily react to. */
	class RocketLeagueEventAdapter
	{
	public:
		void hookToEvents(const std::shared_ptr<GameWrapper>& gameWrapper);

	private:

	};
}