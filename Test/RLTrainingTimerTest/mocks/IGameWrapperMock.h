#pragma once

#include <Plugin/training/control/IGameWrapper.h>
#include <gmock/gmock.h>

class IGameWrapperMock : public IGameWrapper
{
	MOCK_METHOD(void, HookEventPost, (std::string eventName, std::function<void(std::string eventName)> callback), (override));
	MOCK_METHOD(void, HookEvent, (std::string eventName, std::function<void(std::string eventName)> callback), (override));
	MOCK_METHOD(void, Execute, (std::function<void(GameWrapper*)> theLambda), (override));
	MOCK_METHOD(bool, IsPaused, (), (override));
};