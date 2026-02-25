#ifndef ENGINE_EVENTS_EVENT
#define ENGINE_EVENTS_EVENT

// based on https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Events/Event.h

#include "Engine/Core/Base.h"
#include <cstdint>

namespace Engine
{
    enum class EventType {
        None = 0,
        Quit, WindowClosed, WindowResized,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseWheelScrolled
    };

    enum class EventCategory : unsigned int {
        None = 0,
        Application    = BIT(0),
        Window         = BIT(1),
		Input          = BIT(2),
		Keyboard       = BIT(3),
		Mouse          = BIT(4),
		MouseButton    = BIT(5)
    };

    // Event category operators
    constexpr inline EventCategory operator|(EventCategory l, EventCategory r) {
        return static_cast<EventCategory>(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
    }

    constexpr inline EventCategory operator&(EventCategory l, EventCategory r) {
        return static_cast<EventCategory>(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
    }

    constexpr inline EventCategory operator^(EventCategory l, EventCategory r) {
        return static_cast<EventCategory>(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
    }

    constexpr inline EventCategory operator~(EventCategory v) {
        return static_cast<EventCategory>(~static_cast<unsigned int>(v));
    }

    inline EventCategory& operator|=(EventCategory& l, EventCategory r) noexcept {
        l = static_cast<EventCategory>(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
        return l;
    }

    inline EventCategory& operator&=(EventCategory& l, EventCategory r) noexcept {
        l = static_cast<EventCategory>(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
        return l;
    }

    inline EventCategory operator^=(EventCategory& l, EventCategory r) noexcept {
        l = static_cast<EventCategory>(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
        return l;
    }

    #define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return type; }\
								    virtual EventType GetEventType() const override { return GetStaticType(); }\

    #define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategoryFlags() const override { return category; }


    class Event
	{
	public:
		virtual ~Event() = default;

		bool handled = false;

		virtual EventType GetEventType() const = 0;
		virtual EventCategory GetCategoryFlags() const = 0;

		bool IsInCategory(EventCategory category)
		{
			return (GetCategoryFlags() & category) != EventCategory::None;
		}
	};

    class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		
		// F will be deduced by the compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

} // namespace Engine


#endif // ENGINE_EVENTS_EVENT
