#include <pch.h>
#include "InvalidValueException.h"

namespace Core::Kernel
{
	InvalidValueException::InvalidValueException(
		const std::string& domainContext,
		const std::string& domainTypeName, 
		const std::string& domainClassName, 
		const std::string& parameterName, 
		const std::string& invalidityReason,
		const std::string& value)
		: std::runtime_error(fmt::format(
			"Value {} is invalid for parameter {} of {} '{}' (Context: {}): {}",
			value,
			parameterName,
			domainTypeName,
			domainClassName,
			domainContext,
			invalidityReason
		).c_str())
	{
	}
}