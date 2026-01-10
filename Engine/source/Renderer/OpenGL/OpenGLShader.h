#ifndef RENDERER_OPENGL_OPENGLSHADER
#define RENDERER_OPENGL_OPENGLSHADER

#include "Engine/Renderer/IShader.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLShader : public IShader {
    public:
        OpenGLShader(const ShaderDesc& desc);
        ~OpenGLShader();

        void Bind() override;
        void Unbind() override;

    private:
        GLuint m_ProgramID;
    };
}

#endif // RENDERER_OPENGL_OPENGLSHADER
