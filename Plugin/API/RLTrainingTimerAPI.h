#pragma once

#include <string>

/** This interface is used for being able to load training schedules from an external plugin like Prejump.

	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	IF THIS FILE IS CHANGED, THE MAJOR VERSION NUMBER OF RLTRAININGTIMER NEEDS TO BE ADAPTED AND THE OWNERS 
	OF ANY CALLING PLUGIN NEED TO BE INFORMED
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
class RLTrainingTimerAPI
{
protected:
	RLTrainingTimerAPI() = default;

public:
	virtual ~RLTrainingTimerAPI() = default;

	// PURE VIRTUAL METHODS ONLY! NO DATA MEMBERS
	virtual void test() = 0;

	virtual void injectTrainingSchedule(const std::string& uuid, const std::string& jsonData) = 0;
};