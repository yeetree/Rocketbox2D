#include "Renderer/OpenGL/OpenGLPipelineState.h"
#include <iostream>

namespace Engine
{
    OpenGLPipelineState::OpenGLPipelineState(const PipelineDesc& desc) : IPipelineState() {
        m_Shader = desc.shader;
        m_FillMode = desc.fillMode;
        m_CullMode = desc.cullMode;
        m_EnableBlending = desc.enableBlending;
        m_DepthTest = desc.depthTest;
        m_DepthWrite = desc.depthWrite;
    }

    OpenGLPipelineState::~OpenGLPipelineState() {

    }

    void OpenGLPipelineState::Bind() {
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
    }

} // namespace Engine
