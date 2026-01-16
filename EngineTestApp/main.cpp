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


class EngineTestApp : public Application {
public:
    Mat4 viewproj;

    void OnStart() override {
        GetResourceManager().LoadTexture("container", "Assets/awesomeface.png");


        float w = (float)GetWindowWidth();
        float h = (float)GetWindowHeight();

        viewproj = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);

    }

    void OnInput(SDL_Event event) override {
        switch(event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                viewproj = glm::ortho(0.0f, (float)GetWindowWidth(), 0.0f, (float)GetWindowHeight(), -1.0f, 1.0f);
                break;
        }
    }

    void OnUpdate(float dt) override {

    }

    void OnRender() override {

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