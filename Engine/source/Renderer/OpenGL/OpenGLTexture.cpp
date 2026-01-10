#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Engine
{
    OpenGLTexture::OpenGLTexture(const TextureDesc &desc) : m_Format(desc.format), m_Width(desc.width), m_Height(desc.height) {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        // set the texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum internalFormat = 0;
        GLenum dataFormat = 0;
        GLenum dataType = GL_UNSIGNED_BYTE;

        // Get formats
        switch(m_Format) {
            case TextureFormat::RGBA8: 
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
                break;
            case TextureFormat::RGB8:
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
                break;
            case TextureFormat::RGBA16F:
                internalFormat = GL_RGBA16F;
                dataFormat = GL_RGBA;
                dataType = GL_HALF_FLOAT;
                break;
            case TextureFormat::Depth24Stencil8:
                internalFormat = GL_DEPTH24_STENCIL8;
                dataFormat = GL_DEPTH_STENCIL;
                dataType = GL_UNSIGNED_INT_24_8;
                break;
        }

        // Load texture
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, dataType, desc.data);
        
        if (desc.mips > 1) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    OpenGLTexture::~OpenGLTexture() {

    }

    void OpenGLTexture::Bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void OpenGLTexture::Unbind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    uint32_t OpenGLTexture::GetWidth() const {
        return m_Width;
    }

    uint32_t OpenGLTexture::GetHeight() const {
        return m_Height;
    }

    TextureFormat OpenGLTexture::GetFormat() const {
        return m_Format;
    }

} // namespace Engine
