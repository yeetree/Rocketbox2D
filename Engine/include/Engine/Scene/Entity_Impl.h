#ifndef ENGINE_SCENE_ENTITY_IMPL
#define ENGINE_SCENE_ENTITY_IMPL

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine {
    template<typename T>
    bool Entity::HasComponent() const {
        return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
    }

    template<typename T>
    T& Entity::GetComponent() {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args) {
        return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::RemoveComponent() {
        // Basic safety: only try to remove if it exists
        if (HasComponent<T>()) {
            m_Scene->m_Registry.erase<T>(m_EntityHandle);
        }
    }

    template<typename T>
    T& Entity::AddScript() {
        return m_Scene->AddScript<T>(*this);
    }

    template<typename T>
    T* Entity::GetScript() {
        return m_Scene->GetScript<T>(*this);
    }
}

#endif // ENGINE_SCENE_ENTITY_IMPL
