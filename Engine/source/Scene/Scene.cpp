#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/EntityScript.h"
#include "Engine/Scene/Entity.h"
#include <iostream>

namespace Engine
{
    Scene::Scene() {
        // Call Destroy() on destroyed NativeScriptComponents
        m_Registry.on_destroy<NativeScriptComponent>().connect<&OnNativeScriptDestroy>();
    }

    void Scene::Render(Renderer2D& renderer, const Mat4& viewProj) {
        renderer.BeginScene(viewProj);
        
        // Entity + Transform + Sprite
        for (auto [entity, transform, sprite] : m_Registry.view<TransformComponent, SpriteComponent>().each()) {
            if (sprite.texture) {
                renderer.DrawQuad(sprite.texture, transform.position, transform.scale, transform.rotation, sprite.color);
            }
        }

        renderer.EndScene();
    }

    void Scene::Update(float dt) {
        // Entity + NativeScript
        for (auto [entity, nsc] : m_Registry.view<NativeScriptComponent>().each()) {
            // Instantiate script
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, this };
                nsc.Instance->Start();
            }

            nsc.Instance->Update(dt);
        }
    }

    // Create entity, add Transform and Tag by default
    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    // Destroys native scripts
    void Scene::OnNativeScriptDestroy(entt::registry &registry, entt::entity entity) {
        auto& nsc = registry.get<NativeScriptComponent>(entity);
        if (nsc.Instance) {
            nsc.Instance->Destroy();
            nsc.DestroyScript(&nsc);
        }
    }
} // namespace Engine