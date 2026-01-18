#ifndef RENDERER_OPENGL_OPENGLSHADER
#define RENDERER_OPENGL_OPENGLSHADER

#include "Engine/Renderer/RHI/IShader.h"
#include <glad/gl.h>

namespace Engine {
    class OpenGLShader : public IShader {
    public:
        OpenGLShader(const ShaderDesc& desc);
        ~OpenGLShader();

        void Bind() override;
        void Unbind() override;

        // Uniform setters
        void SetBool(const std::string& name, bool value) override;
        void SetInt(const std::string& name, int value) override;
        void SetFloat(const std::string& name, float value) override;
        void SetVec2(const std::string& name, Vec2 value) override;
        void SetVec3(const std::string& name, Vec3 value) override;
        void SetVec4(const std::string& name, Vec4 value) override;
        void SetMat4(const std::string& name, Mat4 value) override;

    private:
        GLuint m_ProgramID;
    };
}

#endif // RENDERER_OPENGL_OPENGLSHADER
