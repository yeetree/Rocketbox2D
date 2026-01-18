#include "Renderer/OpenGL/OpenGLShader.h"
#include "Engine/Core/Log.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    OpenGLShader::OpenGLShader(const ShaderDesc &desc) : IShader() {

        // Create shader
        m_ProgramID = glCreateProgram();

        // Compilation results
        int success;
        char infoLog[512];

        // Shaders
        uint32_t vertexShader = 0, fragmentShader = 0, geometryShader = 0; // no computeShader

        // Vertex shader
        if (desc.sources.find(ShaderStage::Vertex) != desc.sources.end()) {
            // Vertex shader
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            const char* vertexSource = desc.sources.at(ShaderStage::Vertex).c_str();
            glShaderSource(vertexShader, 1, &vertexSource, NULL);
            glCompileShader(vertexShader);

            // Check for shader compile errors
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                LOG_CORE_ERROR("OpenGL: Vertex shader compilation failed! OpenGL: {0}", infoLog);
            }

            glAttachShader(m_ProgramID, vertexShader);
        }

        // Fragment shader
        if (desc.sources.find(ShaderStage::Fragment) != desc.sources.end()) {
            // Fragment shader
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fragmentSource = desc.sources.at(ShaderStage::Fragment).c_str();
            glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            glCompileShader(fragmentShader);
            glAttachShader(m_ProgramID, fragmentShader);

            // Check for shader compile errors
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                LOG_CORE_ERROR("OpenGL: Fragment shader compilation failed! OpenGL: {0}", infoLog);
            }
        }

        // Geometry shader
        if (desc.sources.find(ShaderStage::Geometry) != desc.sources.end()) {
            // Geometry shader
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            const char* geometrySource = desc.sources.at(ShaderStage::Geometry).c_str();
            glShaderSource(geometryShader, 1, &geometrySource, NULL);
            glCompileShader(geometryShader);
            glAttachShader(m_ProgramID, geometryShader);

            // Check for shader compile errors
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                LOG_CORE_ERROR("OpenGL: Geometry shader compilation failed! OpenGL: {0}", infoLog);
            }
        }

        if (desc.sources.find(ShaderStage::Compute) != desc.sources.end()) {
            // Compute shader
            LOG_CORE_ERROR("OpenGL: Compute shader compilation failed! Engine: Compute shaders are not supported in OpenGL API Backend.");
        }

        // Link shaders
        glLinkProgram(m_ProgramID);
        if(vertexShader) glDeleteShader(vertexShader);
        if(fragmentShader) glDeleteShader(fragmentShader);
        if(geometryShader) glDeleteShader(geometryShader);
        // no computeShader

        // Check for linking errors
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
            LOG_CORE_ERROR("OpenGL: Shader linking failed! OpenGL: {0}", infoLog);
        }

        // Bind Engine UBO
        GLuint blockIndex = glGetUniformBlockIndex(m_ProgramID, "u_ViewData");
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(m_ProgramID, blockIndex, 0);
        }
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_ProgramID);
    }

    void OpenGLShader::Bind() {
        glUseProgram(m_ProgramID);
    }

    void OpenGLShader::Unbind() {
        glUseProgram(0);
    }

    void OpenGLShader::SetBool(const std::string& name, bool value) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), (int)value); 
    }

    void OpenGLShader::SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value); 
    }

    void OpenGLShader::SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value); 
    }

    void OpenGLShader::SetVec2(const std::string& name, Vec2 value) {
        glUniform2f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y); 
    }

    void OpenGLShader::SetVec3(const std::string& name, Vec3 value) {
        glUniform3f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z); 
    }

    void OpenGLShader::SetVec4(const std::string& name, Vec4 value) {
        glUniform4f(glGetUniformLocation(m_ProgramID, name.c_str()), value.x, value.y, value.z, value.w); 
    }
    
    void OpenGLShader::SetMat4(const std::string& name, Mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    
} // namespace Engine
