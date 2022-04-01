#pragma once

#include <string>
#include <stdexcept>

#include <DLLImportExport.h>

namespace Core::Kernel
{
	/** C++ Exception for cases where an operation was performed in a state where the operation is not allowed. */
	class RLTT_IMPORT_EXPORT InvalidStateException : public std::runtime_error
	{
	public:
		explicit InvalidStateException(const std::string& message);
	};
}