#ifndef ENGINE_EVENTS_APPLICATIONEVENT
#define ENGINE_EVENTS_APPLICATIONEVENT

#include "Engine/Events/Event.h"

namespace Engine {

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(EventType::WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Application)

	private:
		unsigned int m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};
}

#endif // ENGINE_EVENTS_APPLICATIONEVENT
