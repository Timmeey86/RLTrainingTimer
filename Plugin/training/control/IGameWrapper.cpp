#include "pch.h"
#include "IGameWrapper.h"

void IGameWrapper::HookEventPost(std::string eventName, std::function<void(std::string eventName)> callback)
{
}

void IGameWrapper::HookEvent(std::string eventName, std::function<void(std::string eventName)> callback)
{
}

void IGameWrapper::Execute(std::function<void(GameWrapper*)> theLambda)
{
}

bool IGameWrapper::IsPaused()
{
    return false;
}
