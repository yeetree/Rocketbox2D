#include "Renderer/OpenGL/OpenGLBuffer.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    OpenGLBuffer::OpenGLBuffer(const BufferDesc& desc) : m_Size(desc.size), m_Type(desc.type) {
        m_Target = (desc.type == BufferType::Vertex) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
        
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(m_Target, m_RendererID);
        
        glBufferData(m_Target, desc.size, desc.data, (desc.isDynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

        glBindBuffer(m_Target, 0);
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

    size_t OpenGLBuffer::GetSize() const {
        return m_Size;
    }

    BufferType OpenGLBuffer::GetType() const {
        return m_Type;
    }

    void OpenGLBuffer::UpdateData(const void* data, size_t size, size_t offset)  {
        if (offset + size > m_Size) {
            LOG_CORE_ERROR("Buffer overflow: Trying to write outside of buffer bounds!");
            return;
        }
        
        glBindBuffer(m_Target, m_RendererID);
        glBufferSubData(m_Target, offset, size, data);

        glBindBuffer(m_Target, 0);
    }
} // namespace Engine
