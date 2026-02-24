#ifndef ENGINE_EVENTS_EVENT
#define ENGINE_EVENTS_EVENT

// based on https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Events/Event.h

#include "Engine/Core/Base.h"
#include <cstdint>

namespace Engine
{
    enum class EventType {
        None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseWheelScrolled
    };

    enum class EventCategory : unsigned int {
        None = 0,
        Application    = BIT(0),
		Input          = BIT(1),
		Keyboard       = BIT(2),
		Mouse          = BIT(3),
		MouseButton    = BIT(4)
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

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual EventCategory GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			(GetCategoryFlags() & category) != EventCategory::None;
		}
	};

} // namespace Engine


#endif // ENGINE_EVENTS_EVENT
