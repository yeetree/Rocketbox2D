#ifndef ENGINE_EVENTS_MOUSEEVENT
#define ENGINE_EVENTS_MOUSEEVENT

#include "engine_export.h"

#include "Engine/Core/Hash.h"

#include "Engine/Events/Event.h"

#include "Engine/Input/MouseButton.h"

namespace Engine
{
    // https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Events/MouseEvent.h
    class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: Event(Hash32("MouseMoved")), m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: Event(Hash32("MouseScrolled")), m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetScrollX() const { return m_XOffset; }
		float GetScrollY() const { return m_YOffset; }

	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButton GetMouseButton() const { return m_Button; }

	protected:
		MouseButtonEvent(const StringName& type, const MouseButton button)
			: Event(type), m_Button(button) {}

		MouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseButton button)
			: MouseButtonEvent(Hash32("MouseButtonPressed"), button) {}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseButton button)
			: MouseButtonEvent(Hash32("MouseButtonReleased"), button) {}
	};
}

#endif // ENGINE_EVENTS_MOUSEEVENT
