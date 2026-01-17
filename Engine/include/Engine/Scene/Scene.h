#ifndef ENGINE_SCENE_SCENE
#define ENGINE_SCENE_SCENE

#include "engine_export.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Math/Matrix.h"

#include <entt/entt.hpp>
#include <SDL3/SDL.h>

namespace Engine
{
    // Scene.h
    class ENGINE_EXPORT Scene {
    public:
        Scene();

        Entity CreateEntity(const std::string& name = "Entity");

        void OnInput(SDL_Event event);
        void OnUpdate(float dt);
        void OnRender(const Mat4 &viewProj);

    private:
        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine


#endif // ENGINE_SCENE_SCENE
