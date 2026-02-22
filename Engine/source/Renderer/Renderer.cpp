#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

// Generated
#include "generated/quadShader.h"

// Quad data
const float quadVerts[] = {
    -0.5f,  -0.5f,   0.0f,  0.0f, // Bottom Left
    0.5f,   -0.5f,  1.0f,   0.0f, // Bottom Right
    0.5f,   0.5f,   1.0f,   1.0f, // Top Right
    -0.5f,  0.5f,   0.0f,   1.0   // Top Left
};

const uint16_t quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

namespace Engine
{
    Renderer::Renderer(IGraphicsDevice* device) : m_GraphicsDevice(device) {
        BufferDesc vboDesc, eboDesc;
        
        // Create VBO and EBO for quad
        vboDesc.data = quadVerts;
        vboDesc.isDynamic = false;
        vboDesc.size = sizeof(quadVerts);
        vboDesc.type = BufferType::Vertex;

        eboDesc.data = quadIndices;
        eboDesc.isDynamic = false;
        eboDesc.size = sizeof(quadIndices);
        eboDesc.type = BufferType::Index;

        Ref<IBuffer> vbo = m_GraphicsDevice->CreateBuffer(vboDesc);
        Ref<IBuffer> ebo = m_GraphicsDevice->CreateBuffer(eboDesc);

        VertexLayout vertLayout = VertexLayout{VertexElement(VertexElementType::Vec2, "a_Position"), VertexElement(VertexElementType::Vec2, "a_TexCoord")};

        // Create quad mesh
        m_QuadMesh = CreateRef<Mesh>(vbo, ebo, 6, vertLayout);

        // Create shader
        ShaderDesc shaderDesc;

        // Listen, i PROMISE that k_QuadShaderByteCode_len is divisible by 4
        const uint32_t* shaderData = static_cast<const uint32_t*>(static_cast<const void*>(k_QuadShaderByteCode));
        std::vector<uint32_t> shaderDataVector(shaderData, shaderData + k_QuadShaderByteCode_len / sizeof(uint32_t));
        shaderDesc.modules = {
            ShaderModule{
                .byteCode = shaderDataVector,
                .entryPoints = {
                    {ShaderStage::Vertex, "vertMain"},
                    {ShaderStage::Fragment, "fragMain"}
                }
            }
        };
        Ref<IShader> shader = m_GraphicsDevice->CreateShader(shaderDesc);

        // shader layout
        ShaderLayout shaderLayout = ShaderLayout{
            // Global: Binding 0, Set 0
            ShaderBinding(
                ShaderBindingType::UniformBuffer,"global", 0, 0, 0, 
                { ShaderElement(ShaderDataType::Mat4, "viewProjection") }),
            
            // Local: Binding 0, Set 1
            ShaderBinding(
                ShaderBindingType::UniformBuffer, "local", 1, 0, 1, 
                { ShaderElement(ShaderDataType::Mat4, "transform"), 
                ShaderElement(ShaderDataType::Vec4, "tint") }),
            
            // Texture: Binding 1, Set 1
            ShaderBinding(ShaderBindingType::Sampler, "texture", 2, 1, 1)
        };

        // Create material
        m_QuadMaterial = CreateRef<Material>(shader, shaderLayout);
    }

    void Renderer::Submit(Ref<Mesh> mesh, Ref<Material> material, const Mat4& transform, uint32_t layer) {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.material = material;
        cmd.transform = transform;
        
        // Layer/Shader/Mesh sort key
        cmd.sortKey = ((uint64_t)layer << 48) | ((uint64_t)cmd.material->GetShader()->GetID() << 16) | cmd.mesh->GetID();

        m_CommandQueue.push_back(cmd);
    }

    void Renderer::DrawQuad(const Ref<ITexture>& texture, const Vec4& color, const Mat4& transform, uint32_t layer) {
        auto instance = CreateRef<MaterialInstance>(m_QuadMaterial);
    
        instance->SetTexture("texture", texture);
        instance->Set("tint", color);
        instance->Set("transform", transform);

        Submit(m_QuadMesh, std::static_pointer_cast<Material>(instance), transform, layer);
    }

    void Renderer::BeginScene(const Mat4& viewProjection) {
        m_FrameBuffers.clear();
        m_QuadMaterial->Set("viewProjection", viewProjection);
    }

    void Renderer::EndScene() {
        // Sort by Shader/Mesh sort key (lowest first)
        std::sort(m_CommandQueue.begin(), m_CommandQueue.end(), 
            [](const RenderCommand& a, const RenderCommand& b) {
                return a.sortKey > b.sortKey;
        });

        Flush();
    }

    void Renderer::Flush() {
        uint32_t lastPSO = 0;

        for (auto& cmd : m_CommandQueue) {
            auto pso = GetOrCreatePSO(cmd.mesh, cmd.material);
            if (pso->GetID() != lastPSO) {
                m_GraphicsDevice->BindPipelineState(*pso);
                lastPSO = pso->GetID();
            }

            // Handle Uniform Blocks
            auto& blocks = cmd.material->GetUniformBlocks();
            for (auto& [name, block] : blocks) {
                // Upload data
                //block.Upload(m_GraphicsDevice);
                //m_FrameBuffers.push_back(block.GetUniformBuffer());

                // Bind
                //m_GraphicsDevice->BindUniformBuffer(
                //    *block.GetUniformBuffer(),
                //    block.GetBinding().binding,
                //    block.GetBinding().set
                //);
            }

            // Handle Textures
            for (const auto& [slot, tex] : cmd.material->GetTextures()) {
                const ShaderBinding* binding = cmd.material->GetLayout().GetBindingBySlot(slot);
                if (binding) {
                    m_GraphicsDevice->BindTexture(*tex, binding->binding, binding->set);
                }
            }

            m_GraphicsDevice->SubmitDraw(
                *cmd.mesh->GetVertexBuffer(), 
                *cmd.mesh->GetIndexBuffer(), 
                cmd.mesh->GetIndexCount()
            );
        }
        m_CommandQueue.clear();
    }

    Ref<IPipelineState> Renderer::GetOrCreatePSO(Ref<Mesh> mesh, Ref<Material> material) {
        uint64_t shaderID = material->GetShader()->GetID();
        uint64_t vertLayoutHash = mesh->GetLayout().GetHash();
        uint64_t shaderLayoutHash = material->GetLayout().GetHash();
        
        // XOR Combine for a simple unique key
        uint64_t psoKey = shaderID ^ (vertLayoutHash + 0x9e3779b9 + (shaderID << 6) + (shaderID >> 2));

        if (m_PSOCache.find(psoKey) == m_PSOCache.end()) {
            PipelineDesc desc{};
            desc.shader = material->GetShader().get();
            desc.vertexLayout = mesh->GetLayout();
            desc.shaderLayout = material->GetLayout();
            desc.fillMode = FillMode::Fill;
            desc.cullMode = CullMode::None;
            desc.topology = PrimitiveTopology::TriangleList;
            desc.enableBlending = true;
            
            m_PSOCache[psoKey] = m_GraphicsDevice->CreatePipelineState(desc);
        }

        return m_PSOCache[psoKey];
    }

} // namespace Engine
