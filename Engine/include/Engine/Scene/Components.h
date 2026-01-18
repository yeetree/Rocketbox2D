#ifndef ENGINE_SCENE_COMPONENTS
#define ENGINE_SCENE_COMPONENTS

#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Math/Vector.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Renderer/Camera.h"

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

    struct MeshComponent {
        std::shared_ptr<Mesh> mesh;
    };

    struct MaterialComponent {
        std::shared_ptr<MaterialInstance> material;
        MaterialComponent(std::shared_ptr<Material> mat) : material(std::make_shared<MaterialInstance>(mat)) {};
    };

    struct CameraComponent {
        Camera camera;
        int priority = 0;
        CameraComponent(float verticalSize = 1.0f) : camera(verticalSize) {};
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
