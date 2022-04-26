#include <pch.h>
#include "IndexOutOfBoundsException.h"

namespace Core::Kernel
{
	IndexOutOfBoundsException::IndexOutOfBoundsException(
		const std::string& context,
		const std::string& className,
		const std::string& parameterName,
		int lowerBound,
		int upperBound,
		int parameterValue)
		: std::runtime_error(fmt::format(
			"Value {} is out of bounds [{},{}] for parameter {} of '{}' (Context: {})",
			parameterValue,
			lowerBound,
			upperBound,
			parameterName,
			context,
			className
		).c_str())
	{
	}
}