#include "Renderer/OpenGL/OpenGLBuffer.h"

namespace Engine
{
    OpenGLBuffer::OpenGLBuffer(const BufferDesc& desc) {
        m_Target = (desc.type == BufferType::Vertex) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

        glGenBuffers(1, &m_RendererID);
        glBindBuffer(m_Target, m_RendererID);
        
        glBufferData(m_Target, desc.size, desc.data, (desc.isDynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }

    OpenGLBuffer::~OpenGLBuffer() {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLBuffer::Bind() {
        glBindBuffer(m_Target, m_RendererID);
    }

    void OpenGLBuffer::Unbind() {
        glBindBuffer(m_Target, 0);
    }
} // namespace Engine
