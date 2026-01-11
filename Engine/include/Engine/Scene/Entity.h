#ifndef ENGINE_SCENE_ENTITY
#define ENGINE_SCENE_ENTITY

#include "engine_export.h"

#include <entt/entt.hpp>

namespace Engine
{
    class Scene;

    class ENGINE_EXPORT Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

        entt::entity GetHandle() const;

        template<typename T, typename... Args> T& AddComponent(Args&&... args);
        template<typename T> T& GetComponent();
        template<typename T> bool HasComponent() const;
        template<typename T> void RemoveComponent();
        
        template<typename T> T& AddScript();
        template<typename T> T* GetScript();
        
        void RemoveScript();

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene = nullptr;
    };

} // namespace Engine


#endif // ENGINE_SCENE_ENTITY
