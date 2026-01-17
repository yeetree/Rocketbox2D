#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using namespace Engine;

float fwrap(float x, float min, float max) {
    if (min > max) {
        return fwrap(x, max, min);
    }
    return (x >= 0 ? min : max) + std::fmod(x, max - min);
}

class SpriteScript : public ScriptableEntity {
    void OnStart() override {

    }

    void OnUpdate(float dt) override {
        GetComponent<TransformComponent>().rotation += dt * 6.0;
    }
};


class EngineTestApp : public Application {
public:
    Mat4 viewproj;
    Scene scene;
    Entity entity;
    Entity entity2;

    void OnStart() override {
        GetResourceManager().LoadTexture("container", "Assets/container.jpg");
        GetResourceManager().LoadTexture("face", "Assets/awesomeface.png");


        float w = (float)GetWindowWidth();
        float h = (float)GetWindowHeight();

        viewproj = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);

        entity = scene.CreateEntity();
        entity.AddComponent<SpriteComponent>(GetResourceManager().GetTexture("face"));
        entity.GetComponent<TransformComponent>().position = Vec2(100, 100);
        entity.GetComponent<TransformComponent>().scale = Vec2(100, 100);
        entity.AddComponent<NativeScriptComponent>().Bind<SpriteScript>();

        entity2 = scene.CreateEntity();
        entity2.AddComponent<SpriteComponent>(GetResourceManager().GetTexture("container"));
        entity2.GetComponent<TransformComponent>().position = Vec2(300, 300);
        entity2.GetComponent<TransformComponent>().scale = Vec2(150, 150);
        entity2.AddComponent<NativeScriptComponent>().Bind<SpriteScript>();
    }

    void OnInput(SDL_Event event) override {
        switch(event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                viewproj = glm::ortho(0.0f, (float)GetWindowWidth(), 0.0f, (float)GetWindowHeight(), -1.0f, 1.0f);
                break;
        }
        scene.OnInput(event);
    }

    void OnUpdate(float dt) override {
        scene.OnUpdate(dt);
    }

    void OnRender() override {
        scene.OnRender(viewproj);
    }

    void OnDestroy() override {

    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "EngineTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}