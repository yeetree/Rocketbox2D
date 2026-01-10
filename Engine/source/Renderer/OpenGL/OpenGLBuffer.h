#ifndef RENDERER_OPENGL_OPENGLBUFFER
#define RENDERER_OPENGL_OPENGLBUFFER

#include "Engine/Renderer/IBuffer.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLBuffer : public IBuffer {
    public:
        OpenGLBuffer(const BufferDesc& desc);
        ~OpenGLBuffer();

        void Bind() override;
        void Unbind() override;

    private:
        GLuint m_RendererID;
        GLenum m_Target;
    };
}

#endif // RENDERER_OPENGL_OPENGLBUFFER
