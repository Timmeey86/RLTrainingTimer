#pragma once

#include <Plugin/training/control/ITimeProvider.h>

/** Allows manually adjusting the time (for unit tests, not for the real world...) */
class FakeTimeProvider : public ITimeProvider
{
public:
	std::chrono::steady_clock::time_point now() const override
	{
		return CurrentFakeTime;
	}

	std::chrono::steady_clock::time_point CurrentFakeTime = std::chrono::steady_clock::now();
};