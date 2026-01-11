#ifndef ENGINE_SCENE_COMPONENTS
#define ENGINE_SCENE_COMPONENTS

#include "Engine/Math/Vector.h"
#include "Engine/Renderer/RHI/ITexture.h"

#include <string>
#include <memory>

namespace Engine {
    struct TransformComponent {
        Vec2 position = { 0.0f, 0.0f };
        Vec2 scale = { 100.0f, 100.0f };
        float rotation = 0.0f;
    };

    struct SpriteComponent {
        std::shared_ptr<ITexture> texture;
        Vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

    struct TagComponent {
        std::string tag;
    };

    class EntityScript;

    struct NativeScriptComponent {
        EntityScript* Instance = nullptr;               // Instance

        EntityScript* (*InstantiateScript)();           // "new"
        void (*DestroyScript)(NativeScriptComponent*);  // "delete"

        template<typename T>
        void Bind() {
            InstantiateScript = []() { return static_cast<EntityScript*>(new T()); };                           // Casts to EntityScript* and returns pointer to new function
            DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };  // Deletes instance
        }
    };
}

#endif // ENGINE_SCENE_COMPONENTS
