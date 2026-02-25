#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

using namespace Engine;

float fwrap(float x, float min, float max) {
    if (min > max) {
        return fwrap(x, max, min);
    }
    return (x >= 0 ? min : max) + std::fmod(x, max - min);
}

class SpriteScript : public ScriptableEntity {
    Vec2 velocity;
    float rotSpeed;
    
    void OnStart() override {
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + (rand() % 150);
        
        velocity = Vec2(std::cos(angle) * speed, std::sin(angle) * speed);
        rotSpeed = 1.0f + (static_cast<float>(rand() % 100) / 20.0f);
    }

    void OnUpdate(float dt) override {
        auto& transform = GetComponent<TransformComponent>();
        
        // Move
        transform.position.x += velocity.x * dt;
        transform.position.y += velocity.y * dt;
        
        // Rotate
        transform.rotation +=rotSpeed * dt;

        if (std::abs(transform.position.x) > 400.0f) velocity.x *= -1.0f;
        if (std::abs(transform.position.y) > 300.0f) velocity.y *= -1.0f;
    }
};

class EngineTestApp : public Application {
public:
    Scene scene;

    void OnStart() override {
        scene.OnStart();

        GetGraphicsDevice().SetVSync(true);
        GetResourceManager().LoadTexture("face", "Assets/awesomeface.png");
        Ref<ITexture> tex = GetResourceManager().GetTexture("face");

        // Spawn sprites
        for (int i = 0; i < 1000; i++) {
            Entity e = scene.CreateEntity();
            
            auto& transform = e.GetComponent<TransformComponent>();
            transform.position = Vec2((rand() % 600) - 300, (rand() % 400) - 200);
            transform.scale = Vec2(32, 32);
            
            e.AddComponent<SpriteComponent>(tex, Vec4(1.0f));
            e.AddComponent<NativeScriptComponent>().Bind<SpriteScript>();
        }
    }

    void OnEvent(Event& event) override {
        scene.OnEvent(event);
    }

    void OnUpdate(float dt) override {
        scene.OnUpdate(dt);
        LOG_INFO("FPS: {0}", 1 / dt);
    }

    void OnRender() override {
        scene.OnRender();
    }

    void OnDestroy() override {

    }
};

int Engine::EntryPoint(int argc, char **argv) {
    EngineTestApp app;
    WindowProperties props = {
        .title = "VKTestApp",
        .width = 800,
        .height = 600,
        .api = GraphicsAPI::Vulkan,
        .resizeable = true,
    };
    app.Init(props);
    app.Run();
    return 0;
}