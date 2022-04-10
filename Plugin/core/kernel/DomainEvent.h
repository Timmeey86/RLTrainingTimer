#pragma once

namespace Core::Kernel
{
	/** Abstract event class mainly used for decoupling common code. */
	class DomainEvent
	{
	public:
		virtual ~DomainEvent() = default;
		/** Returns true if it makes sense to store this event persistently, which also means an object can be restored with it (and other events). */
		inline bool shallBeStoredPersistently() const { return _shallBeStoredPersistently; }

		/** Required for serialization. */
		DomainEvent() : DomainEvent(false) {}

	protected:
		explicit DomainEvent(bool shallBeStoredPersistently) 
			: _shallBeStoredPersistently{ shallBeStoredPersistently }
		{}

	private:
		bool _shallBeStoredPersistently;
	};
}