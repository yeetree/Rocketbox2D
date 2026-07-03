#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/RHI/IGraphicsDevice.h"
#include "Engine/RHI/ICommandBuffer.h"
#include "Engine/Math/Matrix.h"
#include "Engine/Events/WindowEvent.h"

namespace Engine
{
    using namespace RHI;

    struct SpriteVertex
    {
        Vec2 inPosition;
        Vec3 inColor;
        Vec2 inTexCoord;
    };

    static const std::vector<SpriteVertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    static const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
    };


    Renderer::Renderer()
    {
        auto gd = Application::Get()->GetServiceLocator()->Get<IGraphicsDevice>();
        auto fs = Application::Get()->GetServiceLocator()->Get<FileSystem>();
        auto win = Application::Get()->GetServiceLocator()->Get<IWindow>();

        TextureDesc depthdesc{
            .width = win->GetWidth(),
            .height = win->GetHeight(),
            .format = PixelFormat::Depth32,
            .usage = TextureUsage::DepthStencil
        };

        m_DepthBuffer = gd->CreateTexture(depthdesc);

        ShaderDesc shdesc{
            .modules = {
                ShaderModule{
                    .spirv = fs->ReadSPV(fs->GetAbsolutePath("./Assets/Shaders/sprite.spv")),
                    .entryPoints = {
                        {ShaderStage::Vertex, "vertMain"},
                        {ShaderStage::Fragment, "fragMain"}
                    }
                }
            }
        };

        m_SpriteShader = gd->CreateShader(shdesc);

        PipelineDesc pdesc {
            .shader = m_SpriteShader,
            .vertexLayout = {
                {VertexElementType::Vec2, "inPosition"},
                {VertexElementType::Vec3, "inColor"},
                {VertexElementType::Vec2, "inTexCoord"}
            },
            .uniformBindings = {
                {0, ShaderStage::Fragment, UniformType::Texture},
            },
            .colorAttachmentFormats = { PixelFormat::RGBA8 },
            .topology = PrimitiveTopology::TriangleList,
            .polygonMode = PolygonMode::Fill,
            .cullMode = CullMode::None,
            .frontFace = FrontFace::Clockwise,
            .blending = true,
            .depthTest = true,
            .depthWrite = true,
            .depthFormat = PixelFormat::Depth32
        };

        m_SpritePipeline = gd->CreatePipeline(pdesc);

        BufferDesc vbdesc{
            .size = vertices.size() * sizeof(SpriteVertex),
            .type = BufferType::Vertex,
            .usage = BufferUsage::Static
        };

        m_SpriteVertices = gd->CreateBuffer(vbdesc);

        BufferDesc ibdesc{
            .size = indices.size() * sizeof(uint16_t),
            .type = BufferType::Index,
            .usage = BufferUsage::Static
        };

        m_SpriteIndices = gd->CreateBuffer(ibdesc);

        ICommandBuffer* init = gd->BeginImmediate();
        init->UploadBuffer(m_SpriteVertices, (void*)vertices.data(), vertices.size() * sizeof(SpriteVertex), 0);
        init->UploadBuffer(m_SpriteIndices, (void*)indices.data(), indices.size() * sizeof(uint16_t), 0);
        gd->EndImmediate(init);
    }

    Renderer::~Renderer()
    {
        
    }

    void Renderer::OnEvent(StringName type, const Event& event) {
        if(type == Hash32("WindowResized"))
        {
            auto gd = Application::Get()->GetServiceLocator()->Get<IGraphicsDevice>();
            const WindowResizedEvent& wr = static_cast<const WindowResizedEvent&>(event);
            if(m_DepthBuffer.IsValid())
            {
                gd->DestroyTexture(m_DepthBuffer);
            }
            TextureDesc depthdesc{
                .width = wr.GetSizeX(),
                .height = wr.GetSizeY(),
                .format = PixelFormat::Depth32,
                .usage = TextureUsage::DepthStencil
            };
            m_DepthBuffer = gd->CreateTexture(depthdesc);
        }
    }

    void Renderer::Begin(RHI::SwapChainHandle sc)
    {
        auto gd = Application::Get()->GetServiceLocator()->Get<IGraphicsDevice>();
        m_CurrentCommandBuffer = gd->BeginPass(sc, {0.0f, 0.0f, 0.0f, 1.0f}, m_DepthBuffer);
        m_CurrentCommandBuffer->BindPipeline(m_SpritePipeline);
        m_CurrentCommandBuffer->BindVertexBuffer(m_SpriteVertices);
        m_CurrentCommandBuffer->BindIndexBuffer(m_SpriteIndices);
    }

    void Renderer::DrawSprite(RHI::TextureHandle tex, Vec2 pos, Vec2 size, float rot)
    {
        m_CurrentCommandBuffer->BindTexture(tex, 0);
        m_CurrentCommandBuffer->DrawIndexed(6);
    }

    void Renderer::End()
    {
        auto gd = Application::Get()->GetServiceLocator()->Get<IGraphicsDevice>();
        gd->EndPass(m_CurrentCommandBuffer);
    }
} // namespace Engine
