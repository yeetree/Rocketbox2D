#ifndef RENDERER_OPENGL_OPENGLVERTEXARRAY
#define RENDERER_OPENGL_OPENGLVERTEXARRAY

#include "Engine/Renderer/RHI/IVertexArray.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLVertexArray : public IVertexArray {
    public:
        OpenGLVertexArray(const VertexArrayDesc& desc);
        ~OpenGLVertexArray();

        void Bind() override;
        void Unbind() override;

    private:
        static GLenum GetGLBaseType(const VertexElementType &t);
        GLuint m_VAO;
        std::shared_ptr<IBuffer> m_VBO;
        std::shared_ptr<IBuffer> m_EBO;
    };
}

#endif // RENDERER_OPENGL_OPENGLVERTEXARRAY
