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

class EngineTestApp : public Engine {
public:
    std::shared_ptr<ITexture> tex;
    Mat4 viewproj;


    float rot = 0;
    float hue = 0;
    Vec4 tint = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

    void Startup() override {
        GetResourceManager().LoadTexture("container", "Assets/awesomeface.png");
        tex = GetResourceManager().GetTexture("container");

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
    }

    void Update(float dt) override {
        rot += 3.14 * dt;
        hue += 0.1 * dt;

        int i = static_cast<int>(hue * 6);
        float s = 1.0f, v = 1.0f;
        float f = hue * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: tint.r = v, tint.g = t, tint.b = p; break;
            case 1: tint.r = q, tint.g = v, tint.b = p; break;
            case 2: tint.r = p, tint.g = v, tint.b = t; break;
            case 3: tint.r = p, tint.g = q, tint.b = v; break;
            case 4: tint.r = t, tint.g = p, tint.b = v; break;
            case 5: tint.r = v, tint.g = p, tint.b = q; break;
        }

    }

    void Render() override {
        

        GetRenderer2D().BeginScene(viewproj);
        GetRenderer2D().DrawQuad(tex, Vec2(400, 300), Vec2(200, 200), rot, tint);
        GetRenderer2D().EndScene();
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