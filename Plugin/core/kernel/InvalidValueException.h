#pragma once

#include <string>
#include <stdexcept>

#include <DLLImportExport.h>

namespace Core::Kernel
{
	/** C++ Exception for cases where an invalid value was passed to a method */
	class RLTT_IMPORT_EXPORT InvalidValueException : std::runtime_error
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