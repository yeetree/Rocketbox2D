#ifndef ENGINE_SCENE_ENTITY
#define ENGINE_SCENE_ENTITY

#include "engine_export.h"

#include "Engine/Scene/Scene.h"

namespace Engine
{
    class ENGINE_EXPORT Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent() {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene = nullptr;
    };
} // namespace Engine


#endif // ENGINE_SCENE_ENTITY
