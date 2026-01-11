#ifndef ENGINE_SCENE_SCENE
#define ENGINE_SCENE_SCENE

#include <entt/entt.hpp>

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Components.h"

namespace Engine {
    class Entity;
    class EntityScript;

    class ENGINE_EXPORT Scene {
    public:
        Scene();

        Entity CreateEntity(const std::string& name = "Entity");
        
        void Render(Renderer2D& renderer, const Mat4& viewProj);
        void Update(float dt);
        void Input(SDL_Event event);

    private:
        // Instantiates EntityScripts if they weren't already
        EntityScript* GetEntityScript(entt::entity entity, NativeScriptComponent& nsc);
        static void OnNativeScriptDestroy(entt::registry &registry, entt::entity entity);

        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine

#endif // ENGINE_SCENE_SCENE
