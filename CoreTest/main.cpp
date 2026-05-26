#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"

using namespace Engine;


class EngineTestApp : public Application {
public:
    Ref<ISwapChain> sc;

    Ref<IGraphicsDevice> gd;
    Ref<IShader> shader;
    Ref<IPipeline> pipe;
    Ref<FileSystem> fs;

    void OnStart() override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        in->MapAction("printFPS", KeyCode::A);
        in->MapAction("immediate", KeyCode::Q);
        in->MapAction("vsync", KeyCode::W);
        in->MapAction("mailbox", KeyCode::E);

        gd = GetServiceLocator()->Get<IGraphicsDevice>();
        Ref<IWindow> win = GetServiceLocator()->Get<IWindow>();
        fs = GetServiceLocator()->Get<FileSystem>();

        SwapChainDesc scdesc{
            .window = win.get(),
            .presentation = PresentMode::VSync,
            .format = TextureFormat::RGBA8
        };

        sc = gd->CreateSwapChain(scdesc);

        ShaderDesc shdesc{
            .modules = {
                ShaderModule{
                    .byteCode = fs->ReadSPV(fs->GetAbsolutePath("./Assets/Shaders/shader.spv")),
                    .entryPoints = {
                        {ShaderStage::Vertex, "vertMain"},
                        {ShaderStage::Fragment, "fragMain"}
                    }
                }
            }
        };

        shader = gd->CreateShader(shdesc);

        PipelineDesc pdesc {
            .shader = shader.get(),
            .topology = PrimitiveTopology::TriangleList,
            .blending = false
        };

        pipe = gd->CreatePipeline(pdesc);
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
        
        cmd->BeginRendering(sc->GetCurrentBackBuffer(), Vec4(0.0f, 0.0f, 0.25f, 1.0f));

        cmd->BindPipeline(pipe.get());
        cmd->Draw(3);

        cmd->EndRendering();
        cmd->End();
        gd->EndSwapChainPass(sc, cmd);

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