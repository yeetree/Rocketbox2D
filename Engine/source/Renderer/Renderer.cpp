#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

layout (std140) uniform u_ViewData {
    mat4 u_ViewProjection;
};

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
        BufferDesc vboDesc, eboDesc, uboDesc;

        // Create UBO
        uboDesc.isDynamic = false;
        uboDesc.size = sizeof(Mat4);
        uboDesc.type = BufferType::Uniform;

        m_UBO = m_GraphicsDevice->CreateBuffer(uboDesc);
        
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

        VertexLayout layout = {VertexElement(VertexElementType::Vec2, "a_Position"), VertexElement(VertexElementType::Vec2, "a_TexCoord")};

        VertexArrayDesc vaoDesc;
        vaoDesc.vbo = vbo;
        vaoDesc.ebo = ebo;
        vaoDesc.layout = layout;

        Ref<IVertexArray> vao = m_GraphicsDevice->CreateVertexArray(vaoDesc);

        // Create quad mesh
        m_QuadMesh = CreateRef<Mesh>(vao, 6, layout);

        // Create shader
        ShaderDesc shaderDesc;
        //shaderDesc.sources[ShaderStage::Vertex] = vertSource;
        //shaderDesc.sources[ShaderStage::Fragment] = fragSource;
        Ref<IShader> shader = m_GraphicsDevice->CreateShader(shaderDesc);

        // Create material
        m_QuadMaterial = CreateRef<Material>(shader);
    }

    void Renderer::Submit(Ref<Mesh> mesh, Ref<Material> material, const Mat4& transform, uint32_t layer) {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.shader = material->GetShader();
        cmd.uniforms = material->GetUniforms(); 
        cmd.textures = material->GetTextures();
        cmd.transform = transform;
        
        // Layer/Shader/Mesh sort key
        cmd.sortKey = ((uint64_t)layer << 48) | ((uint64_t)cmd.shader->GetID() << 16) | cmd.mesh->GetID();

        m_CommandQueue.push_back(cmd);
    }

    void Renderer::Submit(Ref<Mesh> mesh, Ref<MaterialInstance> material, const Mat4& transform, uint32_t layer) {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.shader = material->GetParent()->GetShader();
        cmd.uniforms = material->GetParent()->GetUniforms(); 
        cmd.textures = material->GetParent()->GetTextures();
        cmd.uniformOverrides = material->GetUniformOverrides();
        cmd.textureOverrides = material->GetTextureOverrides();
        cmd.transform = transform;
        
        // Layer/Shader/Mesh sort key
        cmd.sortKey = ((uint64_t)layer << 48) | ((uint64_t)cmd.shader->GetID() << 16) | cmd.mesh->GetID();
        
        m_CommandQueue.push_back(cmd);
    }

    void Renderer::DrawQuad(const Ref<ITexture>& texture, const Vec4& color, const Mat4& transform, uint32_t layer) {
        RenderCommand cmd;
        cmd.mesh = m_QuadMesh;
        cmd.shader = m_QuadMaterial->GetShader();
        cmd.transform = transform;
        
        // Use overrides
        cmd.textureOverrides["u_Texture"] = texture;
        cmd.uniformOverrides["u_Tint"] = color;
        
        cmd.sortKey = ((uint64_t)layer << 48) | ((uint64_t)cmd.shader->GetID() << 16) | cmd.mesh->GetID();
        m_CommandQueue.push_back(cmd);
    }

    void Renderer::BeginScene(const Mat4& viewProjection) {
        m_UBO->BindBase(0);
        m_UBO->UpdateData(glm::value_ptr(viewProjection), sizeof(Mat4), 0);
    }

    void Renderer::EndScene() {
        // Sort by Shader/Mesh sort key (lowest first)
        std::sort(m_CommandQueue.begin(), m_CommandQueue.end(), 
            [](const RenderCommand& a, const RenderCommand& b) {
                return a.sortKey > b.sortKey;
        });

        Flush();
        m_UBO->UnbindBase(0);
    }

    void Renderer::Flush() {
        uint32_t lastPSO = 0;
        uint32_t lastMesh = 0;

        for (auto& cmd : m_CommandQueue) {
            // Only switch Pipeline if different
            auto pso = GetOrCreatePSO(cmd.mesh, cmd.shader);
            uint64_t currentPSO = pso->GetID();
            if (currentPSO != lastPSO) {
                pso->Bind();
                lastPSO = currentPSO;
            }

            // Only switch Mesh if it's different
            uint64_t currentMesh = cmd.mesh->GetID();
            if (currentMesh != lastMesh) {
                cmd.mesh->Bind();
                lastMesh = currentMesh;
            }

            // Bind Material uniforms and textures and overrides
            // Set uniforms
            for (const auto& [name, value] : cmd.uniforms) {
                cmd.shader->Set(name, value);
            }

            for (const auto& [name, value] : cmd.uniformOverrides) {
                cmd.shader->Set(name, value);
            }

            // Bind textures
            uint32_t slot = 0;
            std::unordered_map<std::string, uint32_t> texSlots;
            for (const auto& [name, tex] : cmd.textures) {
                texSlots[name] = slot;
                tex->Bind(slot);
                cmd.shader->SetInt(name, slot);
                slot++;
            }

            for (const auto& [name, tex] : cmd.textureOverrides) {
                uint32_t texSlot = slot;
                if(texSlots.count(name)) {
                    texSlot = texSlots[name];
                }
                else {
                    slot++;
                }
                tex->Bind(texSlot);
                cmd.shader->SetInt(name, texSlot);
            }

            // Set standard Engine uniforms (per object)
            cmd.shader->SetMat4("u_Transform", cmd.transform);

            // Draw
            m_GraphicsDevice->SubmitDraw(cmd.mesh->GetIndexCount());
        }
        m_CommandQueue.clear();
    }

    Ref<IPipelineState> Renderer::GetOrCreatePSO(Ref<Mesh> mesh, Ref<IShader> shader) {
        uint64_t shaderID = shader->GetID();
        uint64_t layoutHash = mesh->GetLayout().GetHash();
        
        // XOR Combine for a simple unique key
        uint64_t psoKey = shaderID ^ (layoutHash + 0x9e3779b9 + (shaderID << 6) + (shaderID >> 2));

        if (m_PSOCache.find(psoKey) == m_PSOCache.end()) {
            PipelineDesc desc{};
            desc.shader = shader.get();
            desc.layout = mesh->GetLayout();
            
            desc.enableBlending = true; // Temp
            
            m_PSOCache[psoKey] = m_GraphicsDevice->CreatePipelineState(desc);
        }

        return m_PSOCache[psoKey];
    }

} // namespace Engine
