#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>

// Quad data
const float quadVerts[] = {
    -0.5f,  -0.5f,   0.0f,  0.0f, // Bottom Left
    0.5f,   -0.5f,  1.0f,   0.0f, // Bottom Right
    0.5f,   0.5f,   1.0f,   1.0f, // Top Right
    -0.5f,  0.5f,   0.0f,   1.0   // Top Left
};

const unsigned int quadIndices[] = {
    0, 1, 2,
    0, 2, 3
};

// Quad shader
const char* vertSource = R"(
#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}

)";

const char* fragSource = R"(
#version 330 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Tint;

out vec4 f_Color;

void main()
{
    f_Color = u_Tint * texture(u_Texture, v_TexCoord);
}

)";

namespace Engine
{
    Renderer::Renderer(IGraphicsDevice* device) : m_GraphicsDevice(device) {
        // Create VBO and EBO for quad
        BufferDesc vboDesc, eboDesc;

        vboDesc.data = quadVerts;
        vboDesc.isDynamic = false;
        vboDesc.size = sizeof(quadVerts);
        vboDesc.type = BufferType::Vertex;

        eboDesc.data = quadIndices;
        eboDesc.isDynamic = false;
        eboDesc.size = sizeof(quadIndices);
        eboDesc.type = BufferType::Index;

        std::shared_ptr<IBuffer> vbo = m_GraphicsDevice->CreateBuffer(vboDesc);
        std::shared_ptr<IBuffer> ebo = m_GraphicsDevice->CreateBuffer(eboDesc);

        // Create quad mesh
        m_QuadMesh = Mesh(vbo, ebo, 6, {VertexElement(VertexElementType::Vec2, "a_Position"), VertexElement(VertexElementType::Vec2, "a_TexCoord")});

        // Create shader
        ShaderDesc shaderDesc;
        shaderDesc.sources[ShaderStage::Vertex] = vertSource;
        shaderDesc.sources[ShaderStage::Fragment] = fragSource;
        std::shared_ptr<IShader> shader = m_GraphicsDevice->CreateShader(shaderDesc);

        // Create material
        m_QuadMaterial = Material(shader);
    }

    void Renderer::Submit(Mesh* mesh, Material* material, const Mat4& transform) {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.shader = material->GetShader();
        cmd.uniforms = material->GetUniforms(); 
        cmd.textures = material->GetTextures();
        cmd.transform = transform;
        
        // Shader/Mesh sort key
        cmd.sortKey = ((uint64_t)material->GetShader()->GetID() << 32) | (uint64_t)mesh->GetID();
        
        m_CommandQueue.push_back(cmd);
    }

    void Renderer::DrawQuad(const std::shared_ptr<ITexture>& texture, const Vec4& color, const Mat4& transform) {
        m_QuadMaterial.SetTexture("u_Texture", texture);
        m_QuadMaterial.Set("u_Tint", color);
        Submit(&m_QuadMesh, &m_QuadMaterial, transform);
    }

    void Renderer::BeginScene(const Mat4& viewProjection) {
        m_ViewProjection = viewProjection;
    }

    void Renderer::EndScene() {
        // Sort by Shader/Mesh sort key
        std::sort(m_CommandQueue.begin(), m_CommandQueue.end(), 
            [](const RenderCommand& a, const RenderCommand& b) {
                return a.sortKey < b.sortKey;
        });

        Flush();
    }

    void Renderer::Flush() {
        for (auto& cmd : m_CommandQueue) {
            // Get PSO
            auto pso = GetOrCreatePSO(cmd.mesh, cmd.shader.get());
            pso->Bind();

            // Apply the Mesh's specific buffers
            cmd.mesh->Bind();
            pso->ApplyVertexLayout();

            // Bind Material uniforms and textures
            // Set uniforms
            for (const auto& [name, value] : cmd.uniforms) {
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, bool>) cmd.shader->SetBool(name, arg);
                    else if constexpr (std::is_same_v<T, int>) cmd.shader->SetInt(name, arg);
                    else if constexpr (std::is_same_v<T, float>) cmd.shader->SetFloat(name, arg);
                    else if constexpr (std::is_same_v<T, Vec2>) cmd.shader->SetVec2(name, arg);
                    else if constexpr (std::is_same_v<T, Vec3>) cmd.shader->SetVec3(name, arg);
                    else if constexpr (std::is_same_v<T, Vec4>) cmd.shader->SetVec4(name, arg);
                    else if constexpr (std::is_same_v<T, Mat4>) cmd.shader->SetMat4(name, arg);
                }, value);
            }

            // Bind textures
            uint32_t slot = 0;
            for (const auto& [name, tex] : cmd.textures) {
                tex->Bind(slot);
                cmd.shader->SetInt(name, slot);
                slot++;
            }
            
            // Set standard Engine uniforms
            cmd.shader->SetMat4("u_ViewProjection", m_ViewProjection);
            cmd.shader->SetMat4("u_Transform", cmd.transform);

            // Deaw
            m_GraphicsDevice->SubmitDraw(cmd.mesh->GetIndexCount());
        }

        m_CommandQueue.clear();
    }

    std::shared_ptr<IPipelineState> Renderer::GetOrCreatePSO(Mesh* mesh, IShader* shader) {
        uint64_t shaderID = shader->GetID();
        uint64_t layoutHash = mesh->GetLayout().GetHash();
        
        // XOR Combine for a simple unique key
        uint64_t psoKey = shaderID ^ (layoutHash + 0x9e3779b9 + (shaderID << 6) + (shaderID >> 2));

        if (m_PSOCache.find(psoKey) == m_PSOCache.end()) {
            PipelineDesc desc{};
            desc.shader = shader;
            desc.layout = mesh->GetLayout();
            
            desc.enableBlending = true; // Temp
            
            m_PSOCache[psoKey] = m_GraphicsDevice->CreatePipelineState(desc);
        }

        return m_PSOCache[psoKey];
    }

} // namespace Engine
