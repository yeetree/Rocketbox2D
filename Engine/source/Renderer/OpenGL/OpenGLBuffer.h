#ifndef RENDERER_OPENGL_OPENGLBUFFER
#define RENDERER_OPENGL_OPENGLBUFFER

#include "Engine/Renderer/RHI/IBuffer.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLBuffer : public IBuffer {
    public:
        OpenGLBuffer(const BufferDesc& desc);
        ~OpenGLBuffer();

        void Bind() override;
        void Unbind() override;

        size_t GetSize() const override;
        BufferType GetType() const override;

        void UpdateData(const void* data, size_t size, size_t offset = 0) override;

    private:
        BufferType m_Type;
        size_t m_Size;
        GLuint m_RendererID;
        GLenum m_Target;
    };
}

#endif // RENDERER_OPENGL_OPENGLBUFFER
