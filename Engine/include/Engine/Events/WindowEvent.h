#ifndef ENGINE_EVENTS_WINDOWEVENT
#define ENGINE_EVENTS_WINDOWEVENT

#include "Engine/Events/Event.h"

namespace Engine
{
    //fwd:
	class IWindow;

    class WindowEvent : public Event
    {
    public:
        IWindow& GetWindow() const { return *m_Window; }
    protected:
        WindowEvent(IWindow* window) : m_Window(window) {}
        IWindow* m_Window;
    };

    class WindowClosedEvent : public WindowEvent
	{
	public:
		WindowClosedEvent(IWindow* window)
			: WindowEvent(window) {};

		EVENT_CLASS_TYPE(EventType::WindowClosed)
		EVENT_CLASS_CATEGORY(EventCategory::Window)
	};

	class WindowResizedEvent : public WindowEvent
	{
	public:
		WindowResizedEvent(IWindow* window, unsigned int width, unsigned int height)
			: WindowEvent(window), m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(EventType::WindowResized)
		EVENT_CLASS_CATEGORY(EventCategory::Window)

	private:
		unsigned int m_Width, m_Height;
	};
} // namespace Engine


#endif // ENGINE_EVENTS_WINDOWEVENT
