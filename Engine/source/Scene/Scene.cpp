#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    Scene::Scene() {
        m_Registry.on_construct<CameraComponent>().connect<&Scene::OnCameraAdded>(this);
    }

    Scene::~Scene() {
        m_Registry.on_construct<CameraComponent>().disconnect<&Scene::OnCameraAdded>(this);
    }

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(name);
        return entity;
    }

    Entity Scene::GetEntity(const std::string& name) {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view) {
            const auto& tag = view.get<TagComponent>(entity);
            if (tag.tag == name) {
                return { entity, this };
            }
        }
        
        LOG_CORE_WARN("Entity with tag '{0}' not found!", name);
        return {};
    }

    void Scene::OnStart() {
        float height, aspect;
        height = (float)Application::Get().GetWindowHeight();
        // Default camera is pixel-perfect
        m_DefaultCamera = Camera(height);
    }

    void Scene::OnInput(SDL_Event event) {
        if(event.type == SDL_EVENT_WINDOW_RESIZED) {
            m_DefaultCamera.OnResize();
            // Entity + CameraComponent
            for (auto [entity, cam] : m_Registry.view<CameraComponent>().each()) {
                // Set aspect ratio
                cam.camera.OnResize();
            };
        }

        // Entity + NativeScriptComponent
        for (auto [entity, nsc] : m_Registry.view<NativeScriptComponent>().each()) {
            // Initialize if needed
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = { entity, this };
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
                nsc.Instance->m_Entity = { entity, this };
                nsc.Instance->OnStart();
            }

            nsc.Instance->OnUpdate(dt);
        };
    }

    void Scene::OnRender() {
        Renderer& renderer = Application::Get().GetRenderer();

        // Get camera
        Entity camera = GetActiveCameraEntity();
    
        if (!camera.IsValid()) {
            renderer.BeginScene(m_DefaultCamera.GetViewProjectionMatrix());
        } else {
            CameraComponent& cam = camera.GetComponent<CameraComponent>();
            TransformComponent& trans = camera.GetComponent<TransformComponent>();
            
            // Sync camera & transform
            cam.camera.SetPosition(trans.position);
            cam.camera.SetRotation(trans.rotation);
            
            renderer.BeginScene(cam.camera.GetViewProjectionMatrix());
        }
        
        // Entity + Transform + Sprite
        for (auto [entity, transform, sprite] : m_Registry.view<TransformComponent, SpriteComponent>().each()) {
            if (sprite.texture) {
                glm::mat4 transformMat = glm::mat4(1.0f);
                transformMat = glm::translate(transformMat, Vec3(transform.position, 0.0f));
                transformMat = glm::rotate(transformMat, transform.rotation, Vec3(0.0f, 0.0f, 1.0f));
                transformMat = glm::scale(transformMat, Vec3(transform.scale, 1.0f));
                renderer.DrawQuad(sprite.texture, sprite.color, transformMat);
            }
        };

        // Entity + Transform + Mesh + Material
        for (auto [entity, transform, mesh, material] : m_Registry.view<TransformComponent, MeshComponent, MaterialComponent>().each()) {
            if (mesh.mesh && material.material) {
                glm::mat4 transformMat = glm::mat4(1.0f);
                transformMat = glm::translate(transformMat, Vec3(transform.position, 0.0f));
                transformMat = glm::rotate(transformMat, transform.rotation, Vec3(0.0f, 0.0f, 1.0f));
                transformMat = glm::scale(transformMat, Vec3(transform.scale, 1.0f));
                renderer.Submit(mesh.mesh, material.material, transformMat);
            }
        };

        renderer.EndScene();
    }

    void Scene::OnCameraAdded(entt::registry& registry, entt::entity entity) {
        registry.get<CameraComponent>(entity).camera.OnResize();
    }

    Entity Scene::GetActiveCameraEntity() {
        Entity selectedCamera;
        int highestPriority = -1;

        // Entity + Camera
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto& cam = view.get<CameraComponent>(entity);
            if (cam.priority > highestPriority) {
                highestPriority = cam.priority;
                selectedCamera = { entity, this };
            }
        }

        return selectedCamera;
    }

} // namespace Engine
