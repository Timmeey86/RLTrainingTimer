#include <pch.h>

#include "InvalidStateException.h"

namespace Core::Kernel
{
	InvalidStateException::InvalidStateException(const std::string& message)
		: std::runtime_error(message)
	{

	}
}
