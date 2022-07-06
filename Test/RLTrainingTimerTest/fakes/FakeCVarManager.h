#pragma once

#include <Plugin/training/control/ICVarManager.h>
#include <unordered_map>
#include <string>

/** This fake class allows reading the most recent command which would have been executed, instead of actually executing it. */
class FakeCVarManager: public ICVarManager
{
public:
	FakeCVarManager() = default;

	void executeCommand(std::string command, bool log) override
	{
		_lastCommand = command;
	}

	inline std::string lastCommand() const { return _lastCommand; }
private:
	std::string _lastCommand;
};