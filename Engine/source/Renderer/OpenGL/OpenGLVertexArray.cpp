#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    GLenum OpenGLVertexArray::GetGLBaseType(const VertexElementType &t) {
        switch(t) {
            case VertexElementType::Float:
            case VertexElementType::Vec2:
            case VertexElementType::Vec3:
            case VertexElementType::Vec4:
            case VertexElementType::Mat4: return GL_FLOAT;
            case VertexElementType::Int:  return GL_INT;
            case VertexElementType::Bool: return GL_BOOL;
            default: return GL_FLOAT;
        }
    } 

    OpenGLVertexArray::OpenGLVertexArray(const VertexArrayDesc &desc) : IVertexArray(), m_VBO(desc.vbo), m_EBO(desc.ebo) {
        // Create VAO
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Bind VBO and EBO
        m_VBO->Bind();
        m_EBO->Bind();

        // Setup vertex attributes
        uint32_t attributeIndex = 0; 
        uint32_t stride = desc.layout.GetStride();
        for (const auto& e : desc.layout.GetElements()) {
            uint32_t count = (e.type == VertexElementType::Mat4) ? 4 : 1;
            for (uint32_t i = 0; i < count; i++) {
                glEnableVertexAttribArray(attributeIndex);
                glVertexAttribPointer(
                    attributeIndex,
                    (e.type == VertexElementType::Mat4) ? 4 : e.components,
                    GetGLBaseType(e.type),
                    e.normalized ? GL_TRUE : GL_FALSE,
                    stride,
                    (void*)(uintptr_t)(e.offset + (i * sizeof(float) * 4))
                );
                attributeIndex++;
            }
        }

        // Unbind VAO
        glBindVertexArray(0);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void OpenGLVertexArray::Bind() {
        // Bind
        glBindVertexArray(m_VAO);
    }

    void OpenGLVertexArray::Unbind() {
        // Unbind
        glBindVertexArray(0);
    }
    
} // namespace Engine
