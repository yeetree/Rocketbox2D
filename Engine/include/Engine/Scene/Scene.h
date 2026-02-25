#ifndef ENGINE_SCENE_SCENE
#define ENGINE_SCENE_SCENE

#include "engine_export.h"

#include "Engine/Math/Matrix.h"
#include "Engine/Renderer/Camera.h"

#include <entt/entt.hpp>

namespace Engine
{
    class Entity;
    class Event;

    // Scene.h
    class ENGINE_EXPORT Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = "Entity");
        Entity GetEntity(const std::string& name);

        void OnStart();
        void OnEvent(Event& event);
        void OnUpdate(float dt);
        void OnRender();

    private:
        void OnCameraAdded(entt::registry& registry, entt::entity entity);
        Entity GetActiveCameraEntity();

        Camera m_DefaultCamera;

        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine


#endif // ENGINE_SCENE_SCENE
