#include "Renderer/OpenGL/OpenGLPipelineState.h"
#include <iostream>

namespace Engine
{
    static GLenum GetGLBaseType(const VertexElementType &t) {
        switch(t) {
            case VertexElementType::Float:
            case VertexElementType::Vec2:
            case VertexElementType::Vec3:
            case VertexElementType::Vec4:
            case VertexElementType::Mat2:
            case VertexElementType::Mat3:
            case VertexElementType::Mat4: return GL_FLOAT;
            case VertexElementType::Int:  return GL_INT;
            case VertexElementType::Bool: return GL_BOOL;
            default: return GL_FLOAT;
        }
    } 

    OpenGLPipelineState::OpenGLPipelineState(const PipelineDesc& desc) {
        m_Shader = desc.shader;
        m_Elements = desc.layout.GetElements();
        m_Stride = desc.layout.GetStride();
        m_FillMode = desc.fillMode;
        m_CullMode = desc.cullMode;
        m_EnableBlending = desc.enableBlending;
        m_DepthTest = desc.depthTest;
        m_DepthWrite = desc.depthWrite;

        glGenVertexArrays(1, &m_VAO);
    }

    OpenGLPipelineState::~OpenGLPipelineState() {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void OpenGLPipelineState::Bind() {
        // Use VAO
        glBindVertexArray(m_VAO);

        // Use shader
        m_Shader->Bind();

        // Set cull, fill, blending, and depth test/write
        glPolygonMode(GL_FRONT_AND_BACK, (m_FillMode == FillMode::Solid) ? GL_FILL : GL_LINE);
        if(m_CullMode == CullMode::None) {
            glDisable(GL_CULL_FACE);
        }
        else {
            glEnable(GL_CULL_FACE);
            glCullFace((m_CullMode == CullMode::Back) ? GL_BACK : GL_FRONT);
        }

        if(m_EnableBlending) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else {
            glDisable(GL_BLEND);
        }

        if(m_DepthTest) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }

        glDepthMask(m_DepthWrite ? GL_TRUE : GL_FALSE);

    };

    void OpenGLPipelineState::Unbind() {
        // Unbind shader & VAO
        m_Shader->Unbind();
        glBindVertexArray(0);
    }

    void OpenGLPipelineState::ApplyVertexLayout() {
        uint32_t attributeIndex = 0; 
        for (const auto& e : m_Elements) {
            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribPointer(
                attributeIndex,
                e.components,
                GetGLBaseType(e.type),
                e.normalized ? GL_TRUE : GL_FALSE,
                m_Stride,
                (void*)(uintptr_t)e.offset
            );
            attributeIndex++;
        }
    }

} // namespace Engine
