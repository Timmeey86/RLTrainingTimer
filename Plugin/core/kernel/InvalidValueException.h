#pragma once

#include <string>
#include <stdexcept>

#include <DLLImportExport.h>

namespace Core::Kernel
{
	/** C++ Exception for cases where an invalid value was passed to a method */
	class RLTT_IMPORT_EXPORT InvalidValueException : public std::runtime_error
	{
	public:
		InvalidValueException(
			const std::string& context,
			const std::string& className,
			const std::string& parameterName,
			const std::string& invalidityReason,
			const std::string& value
		);
	};
}