#pragma once

#include <chrono>

/** This interface allows unit testing classes which track the current time. */
class ITimeProvider
{
protected:
	ITimeProvider() = default;

public:
	virtual ~ITimeProvider() = default;

	/** Retrieves the current point in time. */
	virtual std::chrono::steady_clock::time_point now() const = 0;
};

/** The default time provider to be used when not unit testing. */
class SteadyClockTimeProvider : public ITimeProvider
{
public:
	std::chrono::steady_clock::time_point now() const override
	{
		return std::chrono::steady_clock::now();
	}
};