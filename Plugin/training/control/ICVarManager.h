#pragma once

#include <string>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>

/** This interface allows testing classes which rely on the cvar manager, without having to start Rocket League. */
class ICVarManager
{
protected:
	ICVarManager() = default;

public:

	virtual void executeCommand(std::string command, bool log = true) = 0;
};

/** Since we can't modify CvarManagerWrapper to inherit from the interface above, we provide an adapter to our interface instead. */
class CVarManagerAdapter : public ICVarManager
{
public:
	CVarManagerAdapter(std::shared_ptr<CVarManagerWrapper> actualCVarManager)
		: ICVarManager()
		, _actualCVarManager{ std::move(actualCVarManager) }
	{
	}

	void executeCommand(std::string command, bool log) override
	{
		_actualCVarManager->executeCommand(command, log);
	}

private:
	std::shared_ptr<CVarManagerWrapper> _actualCVarManager;
};