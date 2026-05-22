#ifndef ENGINE_EVENTS_WINDOWEVENT
#define ENGINE_EVENTS_WINDOWEVENT

#include "engine_export.h"

#include "Engine/Core/Hash.h"

#include "Engine/Events/Event.h"

namespace Engine
{
    // TODO: Window IDs

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(uint32_t newX, uint32_t newY)
			: Event(Hash32("WindowMoved")), m_X(newX), m_Y(newY) {}

		uint32_t GetX() const { return m_X; }
        uint32_t GetY() const { return m_Y; }

	private:
		uint32_t m_X, m_Y;
	};

    class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(uint32_t sizeX, uint32_t sizeY)
			: Event(Hash32("WindowResized")), m_SizeX(sizeX), m_SizeY(sizeY) {}

		uint32_t GetSizeX() const { return m_SizeX; }
        uint32_t GetSizeY() const { return m_SizeY; }

	private:
		uint32_t m_SizeX, m_SizeY;
	};
} // namespace Engine


#endif // ENGINE_EVENTS_WINDOWEVENT
