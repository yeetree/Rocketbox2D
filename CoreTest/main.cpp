#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

using namespace Engine;


class EngineTestApp : public Application {
public:
    void OnStart() override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        in->MapAction("testButton", KeyCode::B);
        in->MapAction("testDigitalAxis", KeyCode::C, KeyCode::D);
        in->MapAction("testAnalogAxis", InputAxis::MouseX);
    }

    //void OnEvent(Event& event) override {
    //
    //}

    void OnUpdate(float dt) override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        if(in->IsKeyDown(KeyCode::A))
        {
            LOG_INFO("A press!");
        }
        LOG_INFO("Test: DB: {0}, DA: {1}, AA: {2}", in->IsActionDown("testButton"), in->GetAction("testDigitalAxis"), in->GetAction("testAnalogAxis"));
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