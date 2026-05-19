#ifndef ENGINE_EVENTS_EVENTMANAGER
#define ENGINE_EVENTS_EVENTMANAGER

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Core/StringRegistry.h"

#include "Engine/Events/Event.h"

#include <unordered_map>
#include <functional>
#include <queue>

namespace Engine
{
    using EventCallback = std::function<void(const Event&)>;
    using AllEventCallback = std::function<void(EventType type, const Event&)>;

    using EventListenerID = uint32_t;

    class EventManager
    {
    private:
        struct QueuedEvent
        {
            QueuedEvent(EventType t, Scope<Event> e) : type(t), event(std::move(e)) {};
            EventType type;
            Scope<Event> event;
        };

    
        std::unordered_map<EventType, std::unordered_map<EventListenerID, EventCallback>> m_Subscribers;
        std::unordered_map<EventListenerID, AllEventCallback> m_AllSubscribers;
        std::queue<QueuedEvent> m_EventQueue;
        EventListenerID m_NextListenerID = 0;

    public:
        EventManager() = default;

        EventListenerID Subscribe(EventType type, const EventCallback& callback);
        void Unsubscribe(EventType type, EventListenerID id);

        EventListenerID SubscribeAll(const AllEventCallback& callback);
        void UnsubscribeAll(EventListenerID id);

        // Immediate
        void Dispatch(EventType type, const Event& event);

        // On FlushEvents
        void QueueEvent(EventType type, Scope<Event> event);
        void FlushEvents();
    };
} // namespace Engine


#endif // ENGINE_EVENTS_EVENTMANAGER