#pragma once

#include <string>

namespace training
{
	/** Defines the interface for classes which shall display error messages in any way. */
	class IErrorDisplay
	{
	protected:
		IErrorDisplay() = default;

	public:
		virtual ~IErrorDisplay() = default;

		/** Displays an error message. Implementers must make sure the window displaying the error message is visible. */
		virtual void displayErrorMessage(const std::string& shortTest, const std::string& errorDescription) = 0;

		/** Resets any error message. */
		virtual void clearErrorMessages() = 0;
	};
}