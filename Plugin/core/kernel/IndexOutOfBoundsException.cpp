#include <pch.h>
#include "IndexOutOfBoundsException.h"

namespace Core::Kernel
{
	IndexOutOfBoundsException::IndexOutOfBoundsException(
		const std::string& domainContext,
		const std::string& domainTypeName,
		const std::string& domainClassName,
		const std::string& parameterName,
		int lowerBound,
		int upperBound,
		int parameterValue)
		: std::runtime_error(fmt::format(
			"Value {} is out of bounds [{},{}] for parameter {} of {} '{}' (Context: {})",
			parameterValue,
			lowerBound,
			upperBound,
			parameterName,
			domainTypeName,
			domainClassName,
			domainContext
		).c_str())
	{
	}
}