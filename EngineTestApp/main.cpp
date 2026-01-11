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

class MyScript : public EntityScript {
public:
    float hue;

    void Start() override {
        GetComponent<TransformComponent>().position = Vec2(400, 300);
        GetComponent<TransformComponent>().scale = Vec2(100, 100);
    }

    void Update(float dt) override {
        hue += 0.1 * dt;
        Vec4 &color = GetComponent<SpriteComponent>().color;

        int i = static_cast<int>(hue * 6);
        float s = 1.0f, v = 1.0f;
        float f = hue * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        Vec4 tint(1.0f, 1.0f, 1.0f, 1.0f);

        switch (i % 6) {
            case 0: color.r = v, color.g = t, color.b = p; break;
            case 1: color.r = q, color.g = v, color.b = p; break;
            case 2: color.r = p, color.g = v, color.b = t; break;
            case 3: color.r = p, color.g = q, color.b = v; break;
            case 4: color.r = t, color.g = p, color.b = v; break;
            case 5: color.r = v, color.g = p, color.b = q; break;
        }

        GetComponent<TransformComponent>().rotation += 3.14 * dt;
    }

    void Input(SDL_Event event) override {
        switch(event.type) {
            case SDL_EVENT_KEY_DOWN:
                hue = 0;
                break;
        }
    }
};

class EngineTestApp : public Engine {
public:
    Scene scene;
    Mat4 viewproj;
    Entity entity;

    void Startup() override {
        GetResourceManager().LoadTexture("container", "Assets/awesomeface.png");

        entity = scene.CreateEntity();
        entity.AddComponent<SpriteComponent>(GetResourceManager().GetTexture("container"));
        entity.AddScript<MyScript>();


        float w = (float)GetWindowWidth();
        float h = (float)GetWindowHeight();

        viewproj = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);

    }

    void Input(SDL_Event event) override {
        switch(event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                viewproj = glm::ortho(0.0f, (float)GetWindowWidth(), 0.0f, (float)GetWindowHeight(), -1.0f, 1.0f);
                break;
        }
        scene.Input(event);
    }

    void Update(float dt) override {
        scene.Update(dt);
    }

    void Render() override {
        scene.Render(GetRenderer2D(), viewproj);
    }

    void Cleanup() override {

    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "EngineTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}