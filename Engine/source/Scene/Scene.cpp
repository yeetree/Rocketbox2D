#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Core/Application.h"

namespace Engine
{
    Scene::Scene() {
        
    }

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), &m_Registry };
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(name);
        return entity;
    }

    void Scene::OnInput(SDL_Event event) {
        // Entity + NativeScriptComponent
        for (auto [entity, nsc] : m_Registry.view<NativeScriptComponent>().each()) {
            // Initialize if needed
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity(entity, &m_Registry);
                nsc.Instance->OnStart();
            }

            nsc.Instance->OnInput(event);
        };
    }

    void Scene::OnUpdate(float dt) {
        // Entity + NativeScriptComponent
        for (auto [entity, nsc] : m_Registry.view<NativeScriptComponent>().each()) {
            // Initialize if needed
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, &m_Registry };
                nsc.Instance->OnStart();
            }

            nsc.Instance->OnUpdate(dt);
        };
    }

    void Scene::OnRender(const Mat4 &viewProj) {
        Renderer2D& renderer = Application::Get().GetRenderer2D();

        renderer.BeginScene(viewProj);
        
        // Entity + Transform + Sprite
        for (auto [entity, transform, sprite] : m_Registry.view<TransformComponent, SpriteComponent>().each()) {
            if (sprite.texture) {
                renderer.DrawQuad(sprite.texture, transform.position, transform.scale, transform.rotation, sprite.color);
            }
        };

        renderer.EndScene();
    }

} // namespace Engine
