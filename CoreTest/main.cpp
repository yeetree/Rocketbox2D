#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Assert.h"

using namespace Engine;
using namespace Engine::RHI;

struct Vertex
{
    Vec2 inPosition;
    Vec3 inColor;
};

struct UniformBufferObject
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

class EngineTestApp : public Application {
public:
    Ref<IWindow> win;
    Ref<IGraphicsDevice> gd;
    Ref<FileSystem> fs;

    SwapChainHandle sc;

    ShaderHandle shader;
    PipelineHandle pipeline;

    BufferHandle vb;
    BufferHandle ib;

    UniformBufferObject ubo;

    void OnStart() override {
        Ref<Input> in = GetServiceLocator()->Get<Input>();
        in->MapAction("printFPS", KeyCode::A);
        in->MapAction("immediate", KeyCode::Q);
        in->MapAction("vsync", KeyCode::W);
        in->MapAction("mailbox", KeyCode::E);

        gd = GetServiceLocator()->Get<IGraphicsDevice>();
        win = GetServiceLocator()->Get<IWindow>();
        fs = GetServiceLocator()->Get<FileSystem>();

        SwapChainDesc scdesc{
            .window = win.get(),
            .presentation = PresentMode::VSync,
            .format = PixelFormat::RGBA8
        };

        sc = gd->CreateSwapChain(scdesc);

        ShaderDesc shdesc{
            .modules = {
                ShaderModule{
                    .spirv = fs->ReadSPV(fs->GetAbsolutePath("./Assets/Shaders/shader.spv")),
                    .entryPoints = {
                        {ShaderStage::Vertex, "vertMain"},
                        {ShaderStage::Fragment, "fragMain"}
                    }
                }
            }
        };

        shader = gd->CreateShader(shdesc);

        PipelineDesc pdesc {
            .shader = shader,
            .vertexLayout = {
                {VertexElementType::Vec2, "inPosition"},
                {VertexElementType::Vec3, "inColor"}
            },
            .uniformBindings = {},
            .colorAttachmentFormats = { PixelFormat::RGBA8 },
            .topology = PrimitiveTopology::TriangleList,
            .polygonMode = PolygonMode::Fill,
            .cullMode = CullMode::Back,
            .frontFace = FrontFace::Clockwise,
            .blending = false
        };

        pipeline = gd->CreatePipeline(pdesc);

        
        BufferDesc vbdesc{
            .size = vertices.size() * sizeof(Vertex),
            .type = BufferType::Vertex,
            .usage = BufferUsage::Static
        };

        vb = gd->CreateBuffer(vbdesc);

        BufferDesc ibdesc{
            .size = indices.size() * sizeof(uint16_t),
            .type = BufferType::Index,
            .usage = BufferUsage::Static
        };

        ib = gd->CreateBuffer(ibdesc);

        /*

        BufferDesc ubdesc{
            .size = sizeof(UniformBufferObject),
            .type = BufferType::Uniform,
            .usage = BufferUsage::Dynamic
        };

        ub = gd->CreateBuffer(ubdesc);
        */

        // Upload init data
        ICommandBuffer* init = gd->BeginImmediate();
        init->UploadBuffer(vb, (void*)vertices.data(), vertices.size() * sizeof(Vertex), 0);
        init->UploadBuffer(ib, (void*)indices.data(), indices.size() * sizeof(uint16_t), 0);
        gd->EndImmediate(init);
    }

    void OnEvent(StringName type, const Event& event) override {
        if(type == Hash32("WindowResized"))
        {
            if(sc.IsValid())
            {
                const WindowResizedEvent& wr = static_cast<const WindowResizedEvent&>(event);
                gd->ResizeSwapChain(sc, wr.GetSizeX(), wr.GetSizeY());
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
            gd->SetSwapChainPresentMode(sc, PresentMode::Immediate);
        }
        if(in->IsActionPressed("vsync"))
        {
            gd->SetSwapChainPresentMode(sc, PresentMode::VSync);
        }
        if(in->IsActionPressed("mailbox"))
        {
            gd->SetSwapChainPresentMode(sc, PresentMode::Mailbox);
        }

        static float time = 0;
        time += dt;
        ubo.model = glm::rotate(Mat4(1.0f), time * glm::radians(90.0f), Vec3(0.0f, 0.0f, 1.0f));
        ubo.view = lookAt(Vec3(2.0f, 2.0f, 2.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(win->GetWidth()) / static_cast<float>(win->GetHeight()), 0.1f, 10.0f);
        ubo.proj[1][1] *= -1; // TODO: Fix projection
    }

    void OnRender() override {
        ENGINE_CORE_ASSERT(gd != nullptr, "GD null!");

        gd->BeginFrame();

        ICommandBuffer* cmd = gd->BeginPass(sc, Vec4(0.0f, 0.0f, 0.25f, 1.0f));
        if(cmd)
        {
            cmd->BindPipeline(pipeline);
            cmd->BindVertexBuffer(vb);
            cmd->BindIndexBuffer(ib);
            cmd->DrawIndexed(6);

            gd->EndPass(cmd);
        }
        

        gd->EndFrame();

        //gd->SetBufferData(ub.get(), (void*)&ubo, sizeof(UniformBufferObject));

        //ICommandBuffer* cmd = gd->BeginSwapChainPass(sc.get());
        //cmd->Begin();
        
        //cmd->BeginRendering(sc->GetCurrentBackBuffer(), Vec4(0.0f, 0.0f, 0.25f, 1.0f));

        //cmd->BindPipeline(pipe.get());
        //cmd->BindVertexBuffer(vb.get());
        //cmd->BindIndexBuffer(ib.get());
        //cmd->BindUniformBuffer(ub.get(), pipe.get(), 0);
        //cmd->DrawIndexed(indices.size());

        //cmd->EndRendering(sc->GetCurrentBackBuffer());
        //cmd->End();
        //gd->EndSwapChainPass(sc.get(), cmd);
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