#ifndef ENGINE_SCENE_ENTITY
#define ENGINE_SCENE_ENTITY

#include <entt/entt.hpp>

#include "Engine/Scene/Scene.h"

namespace Engine
{
    class Scene;

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

        // Validation
        bool IsValid() const { 
            return m_EntityHandle != entt::null && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle); 
        }

        // Component Management
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent() {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template<typename T>
        bool HasComponent() const {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }

        Scene& GetScene() {
            return *m_Scene;
        }

        void Destroy() {
            m_Scene->m_Registry.destroy(m_EntityHandle);
            m_EntityHandle = entt::null;
        }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene = nullptr;
    };
} // namespace Engine


#endif // ENGINE_SCENE_ENTITY
