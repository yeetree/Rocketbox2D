#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

using namespace Engine;


class EngineTestApp : public Application {
public:
    void OnStart() override {

    }

    //void OnEvent(Event& event) override {
    //
    //}

    void OnUpdate(float dt) override {

    }

    void OnRender() override {

    }

    void OnDestroy() override {

    }
};

int Engine::EntryPoint(int argc, char **argv) {
    EngineTestApp app;
    WindowProperties props = {
        .title = "CoreTest",
        .width = 800,
        .height = 600,
        .api = GraphicsAPI::Vulkan,
        .resizable = true,
    };
    app.Init(props);
    app.Run();
    return 0;
}