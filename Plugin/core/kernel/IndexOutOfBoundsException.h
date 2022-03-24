#pragma once

#include <string>
#include <stdexcept>

#include <DLLImportExport.h>

namespace Core::Kernel
{
	/** C++ Exception for cases where an index was out of bounds */
	class RLTT_IMPORT_EXPORT IndexOutOfBoundsException : public std::runtime_error
	{
	public:
		IndexOutOfBoundsException(
			const std::string& domainContext,
			const std::string& domainTypeName,
			const std::string& domainClassName,
			const std::string& parameterName,
			int lowerBound,
			int upperBound,
			int parameterValue
		);
	};
}