#pragma once

#include <string>
#include <stdexcept>

namespace Core::Kernel
{
	/** C++ Exception for cases where an invalid value was passed to a method */
	class InvalidValueException : std::runtime_error
	{
	public:
		InvalidValueException(
			const std::string& domainContext,
			const std::string& domainTypeName,
			const std::string& domainClassName,
			const std::string& parameterName,
			const std::string& invalidityReason,
			const std::string& value
		);
	};
}