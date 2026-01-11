#include "Engine/Renderer/Renderer2D.h"
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
    Renderer2D::Renderer2D(IGraphicsDevice* device) : m_GraphicsDevice(device) {
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

        m_QuadVBO = m_GraphicsDevice->CreateBuffer(vboDesc);
        m_QuadEBO = m_GraphicsDevice->CreateBuffer(eboDesc);

        // Create shader
        ShaderDesc shaderDesc;
        shaderDesc.sources[ShaderStage::Vertex] = vertSource;
        shaderDesc.sources[ShaderStage::Fragment] = fragSource;
        m_QuadShader = m_GraphicsDevice->CreateShader(shaderDesc);

        // Create pipeline
        PipelineDesc pipelineDesc{};
        pipelineDesc.shader = m_QuadShader.get();
        pipelineDesc.layout = {VertexElement(VertexElementType::Vec2, "a_Position"), VertexElement(VertexElementType::Vec2, "a_TexCoords")};
        pipelineDesc.enableBlending = true;
        m_QuadPipeline = m_GraphicsDevice->CreatePipelineState(pipelineDesc);

    }

    void Renderer2D::BeginScene(const Mat4& viewProj) {
        // Bind pipeline and buffer objects
        m_QuadPipeline->Bind();
        m_QuadVBO->Bind();
        m_QuadEBO->Bind();
        m_QuadPipeline->ApplyVertexLayout();

        // Set shader uniform
        m_QuadShader->SetMat4("u_ViewProjection", viewProj);
    }

    void Renderer2D::DrawQuad(const std::shared_ptr<ITexture>& tex, const Vec2& pos, const Vec2& size, float rotation, Vec4 tint) {
        // Create transform matrix
        Mat4 transform = Mat4(1.0f);
        transform = glm::translate(transform, Vec3(pos, 0.0f));
        transform = glm::rotate(transform, rotation, Vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, Vec3(size, 1.0f));

        // Set shader uniforms
        m_QuadShader->SetMat4("u_Transform", transform);
        m_QuadShader->SetInt("u_Texture", 0);
        m_QuadShader->SetVec4("u_Tint", tint);

        // Bind texture and draw
        tex->Bind(0);

        m_GraphicsDevice->SubmitDraw(6);
    }

    void Renderer2D::EndScene() {
        m_QuadVBO->Unbind();
        m_QuadEBO->Unbind();
        m_QuadPipeline->Unbind();
    }

} // namespace Engine
