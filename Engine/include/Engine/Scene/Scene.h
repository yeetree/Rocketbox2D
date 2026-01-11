#ifndef ENGINE_SCENE_SCENE
#define ENGINE_SCENE_SCENE

#include <entt/entt.hpp>

#include "Engine/Renderer/Renderer2D.h"

namespace Engine {
    class Entity;

    class ENGINE_EXPORT Scene {
    public:
        Scene();

        Entity CreateEntity(const std::string& name = "Entity");
        
        void Render(Renderer2D& renderer, const Mat4& viewProj);
        void Update(float dt);

    private:
        static void OnNativeScriptDestroy(entt::registry &registry, entt::entity entity);

        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine

#endif // ENGINE_SCENE_SCENE
