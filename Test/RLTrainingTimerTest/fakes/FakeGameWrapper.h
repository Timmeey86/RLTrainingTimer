#pragma once

#include <Plugin/training/control/IGameWrapper.h>
#include <unordered_map>
#include <string>

class FakeGameWrapper : public IGameWrapper
{
public:
	FakeGameWrapper() = default;

	void HookEventPost(std::string eventName, std::function<void(std::string eventName)> callback) override
	{
		FakeEventPostMap.try_emplace(eventName, callback);
	}
	void HookEvent(std::string eventName, std::function<void(std::string eventName)> callback) override
	{
		FakeEventMap.try_emplace(eventName, callback);
	}
	void Execute(std::function<void(GameWrapper*)> theLambda) override
	{
		ExecuteWasCalled = true;
		theLambda(nullptr);
	}
	bool IsPaused() override
	{
		return FakeIsPaused;
	}

	bool ExecuteWasCalled = false;
	bool FakeIsPaused = false;
	std::unordered_map<std::string, std::function<void(std::string)>> FakeEventPostMap;
	std::unordered_map<std::string, std::function<void(std::string)>> FakeEventMap;
};