#ifndef ENGINE_SCENE_SCENE
#define ENGINE_SCENE_SCENE

#include <entt/entt.hpp>

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/EntityScript.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace Engine {
    class ENGINE_EXPORT Scene {
    public:
        Scene();

        Entity CreateEntity(const std::string& name = "Entity");
        
        void Render(Renderer2D& renderer, const Mat4& viewProj);
        void Update(float dt);
        void Input(SDL_Event event);

        template<typename T>
        T& AddScript(Entity entity) {
            auto& nsc = m_Registry.emplace<NativeScriptComponent>(entity.GetHandle());
            nsc.Bind<T>();
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = entity;
            nsc.Instance->Start();
            return *static_cast<T*>(nsc.Instance);
        }

        template<typename T>
        T* GetScript(Entity entity) {
            if (!m_Registry.all_of<NativeScriptComponent>(entity.GetHandle()))
                return nullptr;

            auto& nsc = m_Registry.get<NativeScriptComponent>(entity.GetHandle());
            return static_cast<T*>(nsc.Instance);
        }

        void RemoveScript(Entity entity) {
            m_Registry.remove<NativeScriptComponent>(entity.GetHandle());
        }

    private:
        // NativeScriptComponent on_destroy callback
        static void OnNativeScriptDestroy(entt::registry &registry, entt::entity entity);

        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine

#include "Engine/Scene/Entity_Impl.h" // Keep at bottom pls :)

#endif // ENGINE_SCENE_SCENE
