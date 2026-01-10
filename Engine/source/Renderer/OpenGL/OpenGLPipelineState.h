#ifndef RENDERER_OPENGL_OPENGLPIPELINESTATE
#define RENDERER_OPENGL_OPENGLPIPELINESTATE

#include "Engine/Renderer/IPipelineState.h"
#include <glad/gl.h>

namespace Engine {
    // OpenGL Implementation of IPipelineState
    class OpenGLPipelineState : public IPipelineState {
    public:
        // Constructor: Creates a virtual Pipeline State Object (OpenGL 3.3 doesn't actually have them)
        OpenGLPipelineState(const PipelineDesc& desc);
        ~OpenGLPipelineState();

        void Bind() override;
        void Unbind() override;
        void ApplyVertexLayout() override;

    private:
        GLuint m_VAO;                           // Vertex Array Object for pipeline
        IShader* m_Shader;                      // Shader
        std::vector<VertexElement> m_Elements;  // Vertex Arrtributes
        uint32_t m_Stride;                      // Stride
        FillMode m_FillMode;                    // Fill mode
        CullMode m_CullMode;                    // Cull mode
        bool m_EnableBlending;                  // Blending
        bool m_DepthTest;                       // Depth test
        bool m_DepthWrite;                      // Depth write
    };
}

#endif // RENDERER_OPENGL_OPENGLPIPELINESTATE
