#pragma once

namespace Core::Kernel
{
	/** Abstract event class mainly used for decoupling common code. */
	class DomainEvent
	{
	public:
		virtual ~DomainEvent() = default;
	protected:
		DomainEvent() = default;
	};
}