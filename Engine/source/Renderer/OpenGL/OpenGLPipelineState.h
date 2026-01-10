#ifndef RENDERER_OPENGL_OPENGLPIPELINESTATE
#define RENDERER_OPENGL_OPENGLPIPELINESTATE

#include "Engine/Renderer/IPipelineState.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLPipelineState : public IPipelineState {
    public:
        OpenGLPipelineState(const PipelineDesc& desc);
        ~OpenGLPipelineState();

        void Bind() override;
        void Unbind() override;
    private:
        GLuint m_VAO;
        IShader* m_Shader;
        std::vector<VertexElement> m_Elements;
        uint32_t m_Stride;
        FillMode m_FillMode;
        CullMode m_CullMode;
        bool m_EnableBlending;
        bool m_DepthTest;
        bool m_DepthWrite;
    };
}

#endif // RENDERER_OPENGL_OPENGLPIPELINESTATE
