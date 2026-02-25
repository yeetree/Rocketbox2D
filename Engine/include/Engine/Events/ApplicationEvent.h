#ifndef ENGINE_EVENTS_APPLICATIONEVENT
#define ENGINE_EVENTS_APPLICATIONEVENT

#include "Engine/Events/Event.h"

namespace Engine {
	class QuitEvent : public Event
	{
	public:
		QuitEvent() = default;

		EVENT_CLASS_TYPE(EventType::Quit)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};
}

#endif // ENGINE_EVENTS_APPLICATIONEVENT
