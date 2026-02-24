#ifndef ENGINE_EVENTS_MOUSEEVENT
#define ENGINE_EVENTS_MOUSEEVENT

#include "Engine/Events/Event.h"
#include "Engine/Input/MouseButton.h"

namespace Engine
{
    class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		EVENT_CLASS_TYPE(EventType::MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseWheelScrolledEvent : public Event
	{
	public:
		MouseWheelScrolledEvent(const float x, const float y)
			: m_OffsetX(x), m_OffsetY(y) {}

		float GetXOffset() const { return m_OffsetX; }
		float GetYOffset() const { return m_OffsetY; }

		EVENT_CLASS_TYPE(EventType::MouseWheelScrolled)
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input)
	private:
		float m_OffsetX, m_OffsetY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButton GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input | EventCategory::MouseButton)
	protected:
		MouseButtonEvent(const MouseButton button)
			: m_Button(button) {}

		MouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseButton button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseButton button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
	};

} // namespace Engine


#endif // ENGINE_EVENTS_MOUSEEVENT
