#ifndef ENGINE_EVENTS_KEYEVENT
#define ENGINE_EVENTS_KEYEVENT

#include "engine_export.h"

#include "Engine/Core/Hash.h"

#include "Engine/Events/Event.h"

#include "Engine/Input/KeyCode.h"

namespace Engine
{
    // https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Events/KeyEvent.h
    class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(const StringName& type, const KeyCode keycode)
			: Event(type), m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(Hash32("KeyPressed"), keycode), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(Hash32("KeyReleased"), keycode), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

	private:
		bool m_IsRepeat;
	};
} // namespace Engine


#endif // ENGINE_EVENTS_KEYEVENT
