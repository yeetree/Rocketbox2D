#include "Renderer/OpenGL/OpenGLBuffer.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    GLenum OpenGLBuffer::GetGLBufferType(BufferType type) {
        switch (type)
        {
            case BufferType::Vertex:    return GL_ARRAY_BUFFER; break;
            case BufferType::Index:     return GL_ELEMENT_ARRAY_BUFFER; break;
            case BufferType::Uniform:   return GL_UNIFORM_BUFFER; break;
            default:                    return GL_ARRAY_BUFFER; break;
        }
    }

    OpenGLBuffer::OpenGLBuffer(const BufferDesc& desc) : m_Size(desc.size), m_Type(desc.type) {
        
        
        m_Target = GetGLBufferType(desc.type);
        
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

    void OpenGLBuffer::BindBase(uint32_t index) {
        if (m_Type != BufferType::Uniform) {
            LOG_CORE_WARN("OpenGL: BindBase called on non-indexed buffer type!");
            return;
        }
        glBindBufferBase(m_Target, index, m_RendererID);
    }

    void OpenGLBuffer::UnbindBase(uint32_t index) {
        if (m_Type != BufferType::Uniform) {
            LOG_CORE_WARN("OpenGL: UnbindBase called on non-indexed buffer type!");
            return;
        }
        glBindBufferBase(m_Target, index, 0);
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
    }
} // namespace Engine
