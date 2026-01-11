#ifndef RENDERER_OPENGL_OPENGLTEXTURE
#define RENDERER_OPENGL_OPENGLTEXTURE

#include "Engine/Renderer/RHI/ITexture.h"
#include <glad/gl.h>


namespace Engine
{
    class OpenGLTexture : public ITexture {
    public:
        OpenGLTexture(const TextureDesc &desc);
        ~OpenGLTexture();

        void Bind(uint32_t slot = 0) override;
        void Unbind(uint32_t slot = 0) override;

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
        TextureFormat GetFormat() const override;
    private:
        GLuint m_TextureID;
        TextureFormat m_Format;
        uint32_t m_Width, m_Height;
    };
} // namespace Engine


#endif // RENDERER_OPENGL_OPENGLTEXTURE
