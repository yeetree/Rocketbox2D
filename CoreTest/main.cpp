#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Assert.h"

using namespace Engine;

struct Vertex
{
    Vec2 inPosition;
    Vec3 inColor;
};

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

class EngineTestApp : public Application {
public:
    Ref<ISwapChain> sc;

    Ref<IGraphicsDevice> gd;
    Ref<IShader> shader;
    Ref<IPipeline> pipe;
    Ref<FileSystem> fs;

    Ref<IBuffer> buf;

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
            .vertexLayout = {
                { VertexElementType::Vec2, "inPosition" },
                { VertexElementType::Vec3, "inColor" }
            },
            .topology = PrimitiveTopology::TriangleList,
            .blending = false
        };

        pipe = gd->CreatePipeline(pdesc);

        BufferDesc bdesc{
            .size = vertices.size() * sizeof(Vertex),
            .type = BufferType::Vertex,
            .usage = BufferUsage::Static
        };

        buf = gd->CreateBuffer(bdesc);

        // Upload init data
        ICommandBuffer* init = gd->BeginSingleTimeCommands();
        init->Begin();
        init->SetBufferData(buf.get(), (void*)vertices.data(), buf->GetSize(), 0);
        init->End();
        gd->EndSingleTimeCommands(init);
    }

    void OnEvent(StringName type, const Event& event) override {
        if(type == Hash32("WindowResized"))
        {
            if(sc)
            {
                const WindowResizedEvent& wr = static_cast<const WindowResizedEvent&>(event);
                gd->ResizeSwapChain(sc.get(), wr.GetSizeX(), wr.GetSizeY());
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
            gd->SetSwapChainPresentation(sc.get(), PresentMode::Immediate);
        }
        if(in->IsActionPressed("vsync"))
        {
            gd->SetSwapChainPresentation(sc.get(), PresentMode::VSync);
        }
        if(in->IsActionPressed("mailbox"))
        {
            gd->SetSwapChainPresentation(sc.get(), PresentMode::Mailbox);
        }
    }

    void OnRender() override {
        ENGINE_CORE_ASSERT(gd != nullptr, "GD null!");

        gd->BeginFrame();

        ICommandBuffer* cmd = gd->BeginSwapChainPass(sc.get());
        cmd->Begin();
        
        cmd->BeginRendering(sc->GetCurrentBackBuffer(), Vec4(0.0f, 0.0f, 0.25f, 1.0f));

        cmd->BindPipeline(pipe.get());
        cmd->BindVertexBuffer(buf.get());
        cmd->Draw(3);

        cmd->EndRendering(sc->GetCurrentBackBuffer());
        cmd->End();
        gd->EndSwapChainPass(sc.get(), cmd);

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