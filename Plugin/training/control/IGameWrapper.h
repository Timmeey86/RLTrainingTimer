#pragma once

#include <string>
#include <functional>
#include <bakkesmod/wrappers/gamewrapper.h>

/** This interface allows testing classes which rely on the game wrapper, without having to start Rocket League.
 *
 * Note for reviewers: The actual tests are on GitHub, but not part of the solution you are reviewing.
 */
class IGameWrapper
{
protected:
	IGameWrapper() = default;

public:
	virtual ~IGameWrapper() = default;

	virtual void HookEventPost(std::string eventName, std::function<void(std::string eventName)> callback) = 0;
	virtual void HookEvent(std::string eventName, std::function<void(std::string eventName)> callback) = 0;
	virtual void Execute(std::function<void(GameWrapper*)> theLambda) = 0;
	virtual bool IsPaused() = 0;
};

/** Since we can't modify game wrapper to inherit from the interface above, we provide an adapter to our interface instead. */
class GameWrapperAdapter : public IGameWrapper
{
public:
	GameWrapperAdapter(std::shared_ptr<GameWrapper> actualGameWrapper) 
		: IGameWrapper()
		, _actualGameWrapper{ actualGameWrapper }
	{
	}

	void HookEventPost(std::string eventName, std::function<void(std::string eventName)> callback) override
	{
		_actualGameWrapper->HookEventPost(eventName, std::move(callback));
	}
	void HookEvent(std::string eventName, std::function<void(std::string eventName)> callback) override
	{
		_actualGameWrapper->HookEvent(eventName, std::move(callback));
	}
	void Execute(std::function<void(GameWrapper*)> theLambda) override
	{
		_actualGameWrapper->Execute(std::move(theLambda));
	}
	bool IsPaused() override
	{
		return _actualGameWrapper->IsPaused();
	}

private:
	std::shared_ptr<GameWrapper> _actualGameWrapper;
};