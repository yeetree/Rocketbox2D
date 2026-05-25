#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"

using namespace Engine;


class EngineTestApp : public Application {
public:
    Ref<ISwapChain> sc;
    Ref<ISwapChain> sc2;
    Ref<IGraphicsDevice> gd;
    Ref<IWindow> win2;

    void OnStart() override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        in->MapAction("printFPS", KeyCode::A);
        in->MapAction("immediate", KeyCode::Q);
        in->MapAction("vsync", KeyCode::W);
        in->MapAction("mailbox", KeyCode::E);

        gd = GetServiceLocator()->Get<IGraphicsDevice>();
        Ref<IWindow> win = GetServiceLocator()->Get<IWindow>();

        win2 = GetServiceLocator()->Get<IPlatform>()->CreateWindow({
            .title = "Test!",
            .width = 200,
            .height = 200,
            .api = GraphicsAPI::Vulkan,
            .resizable = true,
        });

        SwapChainDesc desc{
            .window = win,
            .presentation = PresentMode::VSync,
            .format = TextureFormat::RGBA8
        };

        sc = gd->CreateSwapChain(desc);

        desc.window = win2;
        sc2 = gd->CreateSwapChain(desc);
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
        gd->BeginFrame();

        ICommandBuffer* cmd = gd->BeginSwapChainPass(sc);

        cmd->Begin();
        cmd->BeginRendering(sc->GetCurrentBackBuffer(), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        cmd->EndRendering();
        cmd->End();

        gd->EndSwapChainPass(sc, cmd);

        ICommandBuffer* cmd2 = gd->BeginSwapChainPass(sc2);

        cmd2->Begin();
        cmd2->BeginRendering(sc2->GetCurrentBackBuffer(), Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        cmd2->EndRendering();
        cmd2->End();

        gd->EndSwapChainPass(sc2, cmd2);

        gd->EndFrame();
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