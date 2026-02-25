#ifndef ENGINE_EVENTS_KEYEVENT
#define ENGINE_EVENTS_KEYEVENT

#include "Engine/Events/Event.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Input/KeyCode.h"

namespace Engine {

	//fwd
	class IWindow;

    class KeyEvent : public WindowEvent {
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
	protected:
		KeyEvent(IWindow* window, const KeyCode keycode)
			: WindowEvent(window), m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(IWindow* window, const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(window, keycode), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

		EVENT_CLASS_TYPE(EventType::KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(IWindow* window, const KeyCode keycode)
			: KeyEvent(window, keycode) {}

		EVENT_CLASS_TYPE(EventType::KeyReleased)
	};
    
} // namespace Engine

#endif // ENGINE_EVENTS_KEYEVENT
