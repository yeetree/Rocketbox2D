#ifndef ENGINE_EVENTS_EVENTMANAGER
#define ENGINE_EVENTS_EVENTMANAGER

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Core/StringName.h"

#include "Engine/Events/Event.h"

#include <unordered_map>
#include <functional>
#include <queue>

namespace Engine
{
    using EventCallback = std::function<void(const Event&)>;
    using AllEventCallback = std::function<void(StringName type, const Event&)>;

    using EventListenerID = size_t;

    class EventManager
    {
    private:
        struct QueuedEvent
        {
            QueuedEvent(StringName t, Scope<Event> e) : type(t), event(std::move(e)) {};
            StringName type;
            Scope<Event> event;
        };

    
        std::unordered_map<StringName, std::unordered_map<EventListenerID, EventCallback>> m_Subscribers;
        std::unordered_map<EventListenerID, AllEventCallback> m_AllSubscribers;
        std::queue<QueuedEvent> m_EventQueue;
        EventListenerID m_NextListenerID = 0;

    public:
        EventManager() = default;

        EventListenerID Subscribe(StringName type, const EventCallback& callback);
        void Unsubscribe(StringName type, EventListenerID id);

        EventListenerID SubscribeAll(const AllEventCallback& callback);
        void UnsubscribeAll(EventListenerID id);

        // Immediate
        void Dispatch(StringName type, const Event& event);

        // On FlushEvents
        void QueueEvent(StringName type, Scope<Event> event);
        void FlushEvents();
    };
} // namespace Engine


#endif // ENGINE_EVENTS_EVENTMANAGER