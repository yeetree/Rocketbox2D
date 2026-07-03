#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Assert.h"

#include "Engine/Renderer/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Engine;
using namespace Engine::RHI;

class TextureResource : public IResource {
public:
    TextureResource(TextureHandle tex) : texture(tex) {}
    ~TextureResource() = default;
    TextureHandle texture;
};

struct TextureLoadDesc : public ResourceLoadDesc {
    TextureLoadDesc(std::string path) : path(path) {}
    ~TextureLoadDesc() = default;
    std::string path;
};

class TextureLoader : public IResourceLoader {
private:
    IGraphicsDevice* m_gd;

public:
    TextureLoader(IGraphicsDevice* graphicsDevice) : m_gd(graphicsDevice) {}
    ~TextureLoader() = default;

    Scope<IResource> Load(const ResourceLoadDesc& desc) override
    {
        const TextureLoadDesc& tdesc = static_cast<const TextureLoadDesc&>(desc);
        int w, h;
        void* texdata = stbi_load(tdesc.path.c_str(), &w, &h, nullptr, 4);

        TextureDesc texdesc{
            .width = static_cast<uint32_t>(w),
            .height = static_cast<uint32_t>(h),
            .format = PixelFormat::RGBA8,
            .usage = TextureUsage::Sampled
        };

        TextureHandle handle = m_gd->CreateTexture(texdesc);

        ICommandBuffer* init = m_gd->BeginImmediate();
        init->UploadTexture(handle, texdata);
        m_gd->EndImmediate(init);
        stbi_image_free(texdata);

        return CreateScope<TextureResource>(handle);
    }

    Scope<IResource> Create(const ResourceCreateDesc& desc) override
    {
        return nullptr;
    }

    void Unload(Scope<IResource> resource)
    {
        TextureResource* tex = static_cast<TextureResource*>(resource.get());
        m_gd->DestroyTexture(tex->texture);
        resource.reset();
    }
};

class EngineTestApp : public Application {
public:
    IWindow* win;
    IGraphicsDevice* gd;
    FileSystem* fs;
    ResourceManager* rm;

    Scope<Renderer> renderer;

    //TextureHandle tex;
    ResourceHandle<TextureResource> tex;

    void OnStart() override {
        Input* in = GetServiceLocator()->Get<Input>();
        in->MapAction("printFPS", KeyCode::A);
        in->MapAction("immediate", KeyCode::Q);
        in->MapAction("vsync", KeyCode::W);
        in->MapAction("mailbox", KeyCode::E);

        gd = GetServiceLocator()->Get<IGraphicsDevice>();
        win = GetServiceLocator()->Get<IWindow>();
        fs = GetServiceLocator()->Get<FileSystem>();
        rm = GetServiceLocator()->Get<ResourceManager>();

        rm->RegisterLoader<TextureResource>(CreateScope<TextureLoader>(gd));
        tex = rm->Load<TextureResource>("awesomeface", TextureLoadDesc(fs->GetAbsolutePath("./Assets/Textures/awesomeface.png")));

        renderer = CreateScope<Renderer>();
    }

    void OnEvent(StringName type, const Event& event) override {
        renderer->OnEvent(type, event);
    }

    void OnUpdate(float dt) override {
        Input* in = GetServiceLocator()->Get<Input>();
        if(in->IsActionPressed("printFPS"))
        {
            LOG_INFO("FPS: {0}", 1 / dt);
        }
        if(in->IsActionPressed("immediate"))
        {
            gd->SetSwapChainPresentMode(GetSwapChain(), PresentMode::Immediate);
        }
        if(in->IsActionPressed("vsync"))
        {
            gd->SetSwapChainPresentMode(GetSwapChain(), PresentMode::VSync);
        }
        if(in->IsActionPressed("mailbox"))
        {
            gd->SetSwapChainPresentMode(GetSwapChain(), PresentMode::Mailbox);
        }
    }

    void OnRender() override {
        ENGINE_CORE_ASSERT(gd != nullptr, "GD null!");

        /*ICommandBuffer* cmd = gd->BeginPass(sc, Vec4(0.0f, 0.0f, 0.25f, 1.0f), depth);
        if(cmd)
        {
            cmd->UploadBuffer(ub, (void*)&ubo, sizeof(UniformBufferObject), 0);

            cmd->BindPipeline(pipeline);
            cmd->BindVertexBuffer(vb);
            cmd->BindIndexBuffer(ib);
            cmd->BindUniformBuffer(ub, 0);
            cmd->BindTexture(rm->Get<TextureResource>(tex)->texture, 1);
            cmd->DrawIndexed(12);

            gd->EndPass(cmd);
        }*/

        renderer->Begin(GetSwapChain());
        renderer->DrawSprite(rm->Get<TextureResource>(tex)->texture, {0, 0}, {100, 100}, 0);
        renderer->End();

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