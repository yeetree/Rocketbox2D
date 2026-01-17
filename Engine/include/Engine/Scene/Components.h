#ifndef ENGINE_SCENE_COMPONENTS
#define ENGINE_SCENE_COMPONENTS

#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Math/Vector.h"
#include "Engine/Renderer/RHI/ITexture.h"

#include <string>
#include <memory>

namespace Engine
{
    struct TransformComponent {
        Vec2 position{0.0f, 0.0f};
        Vec2 scale{1.0f, 1.0f};
        float rotation{0.0f};
    };

    struct TagComponent {
        std::string tag;
    };

    struct SpriteComponent {
        std::shared_ptr<ITexture> texture;
        Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct NativeScriptComponent {
        ScriptableEntity* Instance = nullptr;

        // Function pointers
        ScriptableEntity* (*InstantiateScript)();
        void (*DestroyScript)(NativeScriptComponent*);

        template<typename T>
        void Bind() {
            InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
            DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };

} // namespace Engine


#endif // ENGINE_SCENE_COMPONENTS
