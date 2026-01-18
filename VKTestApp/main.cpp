#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <cmath>

using namespace Engine;


class EngineTestApp : public Application {
public:
    void OnStart() override {
       
    }

    void OnInput(SDL_Event event) override {

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
    app.Init(800, 600, "VKTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}