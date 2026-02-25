#ifndef ENGINE_EVENTS_MOUSEEVENT
#define ENGINE_EVENTS_MOUSEEVENT

#include "Engine/Events/Event.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Input/MouseButton.h"

namespace Engine
{
	class IWindow;

    class MouseMovedEvent : public WindowEvent
	{
	public:
		MouseMovedEvent(IWindow* window, const float x, const float y, const float rx, const float ry)
			: WindowEvent(window), m_MouseX(x), m_MouseY(y), m_RelX(rx), m_RelY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }
		float GetRelX() const { return m_RelX; }
		float GetRelY() const { return m_RelY; }

		EVENT_CLASS_TYPE(EventType::MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
	private:
		float m_MouseX, m_MouseY, m_RelX, m_RelY;
	};

	class MouseWheelScrolledEvent : public WindowEvent
	{
	public:
		MouseWheelScrolledEvent(IWindow* window, const float x, const float y)
			: WindowEvent(window), m_OffsetX(x), m_OffsetY(y) {}

		float GetOffsetX() const { return m_OffsetX; }
		float GetOffsetY() const { return m_OffsetY; }

		EVENT_CLASS_TYPE(EventType::MouseWheelScrolled)
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
	private:
		float m_OffsetX, m_OffsetY;
	};

	class MouseButtonEvent : public WindowEvent
	{
	public:
		MouseButton GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton)
	protected:
		MouseButtonEvent(IWindow* window, const MouseButton button)
			: WindowEvent(window), m_Button(button) {}

		MouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(IWindow* window, const MouseButton button)
			: MouseButtonEvent(window, button) {}

		EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(IWindow* window, const MouseButton button)
			: MouseButtonEvent(window, button) {}

		EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
	};

} // namespace Engine


#endif // ENGINE_EVENTS_MOUSEEVENT
