#ifndef ENGINE_EVENTS_KEYEVENT
#define ENGINE_EVENTS_KEYEVENT

#include "Engine/Events/Event.h"
#include "Engine/Input/KeyCode.h"

namespace Engine {

    class KeyEvent : public Event {
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
	protected:
		KeyEvent(const KeyCode keycode)
			: m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

		EVENT_CLASS_TYPE(EventType::KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(EventType::KeyReleased)
	};
    
} // namespace Engine

#endif // ENGINE_EVENTS_KEYEVENT
