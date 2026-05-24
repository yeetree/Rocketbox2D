#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"

using namespace Engine;


class EngineTestApp : public Application {
public:
    Scope<ISwapChain> sc;

    void OnStart() override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        in->MapAction("printFPS", KeyCode::A);
        in->MapAction("immediate", KeyCode::Q);
        in->MapAction("vsync", KeyCode::W);
        in->MapAction("mailbox", KeyCode::E);

        Ref<IGraphicsDevice> gd = GetServiceLocator()->Get<IGraphicsDevice>();
        Ref<IWindow> win = GetServiceLocator()->Get<IWindow>();

        SwapChainDesc desc{
            .width = win->GetWidth(),
            .height = win->GetHeight(),
            .presentation = PresentMode::VSync,
            .format = TextureFormat::RGBA8
        };

        sc = gd->CreateSwapChain(desc);
    }

    void OnEvent(StringName type, const Event& event) override {
        if(type == Hash32("WindowResized"))
        {
            if(sc)
            {
                const WindowResizedEvent& wr = static_cast<const WindowResizedEvent&>(event);
                sc->Resize(wr.GetSizeX(), wr.GetSizeY());
            }
        }
    }

    void OnUpdate(float dt) override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        if(in->IsActionPressed("printFPS"))
        {
            LOG_INFO("FPS: {0}", 1 / dt);
        }
        if(in->IsActionPressed("immediate"))
        {
            sc->SetPresentation(PresentMode::Immediate);
        }
        if(in->IsActionPressed("vsync"))
        {
            sc->SetPresentation(PresentMode::VSync);
        }
        if(in->IsActionPressed("mailbox"))
        {
            sc->SetPresentation(PresentMode::Mailbox);
        }
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