#include "Engine/Events/EventManager.h"

namespace Engine
{
    EventListenerID EventManager::Subscribe(EventType type, const EventCallback& callback)
    {
        m_Subscribers[type][m_NextListenerID] = callback;
        return m_NextListenerID++;
    }

    void EventManager::Unsubscribe(EventType type, EventListenerID id)
    {
        auto typeIt = m_Subscribers.find(type);
        if(typeIt != m_Subscribers.end())
        {
            auto subIt = typeIt->second.find(id);
            if(subIt != typeIt->second.end())
            {
                typeIt->second.erase(subIt);
            }
        }
    }

    EventListenerID EventManager::SubscribeAll(const AllEventCallback& callback)
    {
        m_AllSubscribers[m_NextListenerID] = callback;
        return m_NextListenerID++;
    }

    void EventManager::UnsubscribeAll(EventListenerID id)
    {
        auto subIt = m_AllSubscribers.find(id);
        if(subIt != m_AllSubscribers.end())
        {
            m_AllSubscribers.erase(subIt);
        }
    }

    void EventManager::Dispatch(EventType type, const Event& event)
    {
        auto typeIt = m_Subscribers.find(type);
        if(typeIt != m_Subscribers.end())
        {
            for(auto const& [id, callback] : typeIt->second)
            {
                callback(event);
            }
        }

        for(auto const& [id, callback] : m_AllSubscribers)
        {
            callback(type, event);
        }
    }

    // On FlushEvents
    void EventManager::QueueEvent(EventType type, Scope<Event> event)
    {
        m_EventQueue.emplace(type, std::move(event));
    }

    void EventManager::FlushEvents()
    {
        while(!m_EventQueue.empty())
        {
            QueuedEvent& event = m_EventQueue.front();
            Dispatch(event.type, *event.event);
            m_EventQueue.pop();
        }
    }
} // namespace Engine
