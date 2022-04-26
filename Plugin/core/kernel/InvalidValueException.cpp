#include <pch.h>
#include "InvalidValueException.h"

namespace Core::Kernel
{
	InvalidValueException::InvalidValueException(
		const std::string& context,
		const std::string& className,
		const std::string& parameterName, 
		const std::string& invalidityReason,
		const std::string& value)
		: std::runtime_error(fmt::format(
			"Value {} is invalid for parameter {} of '{}' (Context: {}): {}",
			value,
			parameterName,
			context,
			className,
			invalidityReason
		).c_str())
	{
	}
}