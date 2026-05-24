#ifndef ENGINE_RHI_ITEXTURE
#define ENGINE_RHI_ITEXTURE

#include "engine_export.h"

#include <cstdint>

namespace Engine {

    // Texture pixel format
    enum class TextureFormat { RGBA8 };

    // Texture usage
    enum TextureUsage {
        Sampled         = 1,
        RenderTarget    = 1 << 1
    };

    // Decribes how a texture should be created
    struct TextureDesc {
        uint32_t width = 0;
        uint32_t height = 0;
        // uint32_t mips = 1;
        TextureFormat format = TextureFormat::RGBA8;
        TextureUsage usage = TextureUsage::Sampled;
    };

    // ITexture represents a texture in GPU memory.
    class ENGINE_EXPORT ITexture {
    private:
        uint32_t m_Width, m_Height;
        TextureFormat m_Format;
        uint32_t m_ID;

    protected:
        ITexture(uint32_t width, uint32_t height, TextureFormat format) : m_Width(width), m_Height(height), m_Format(format) {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }
    
    public:
        virtual ~ITexture() = default;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        TextureFormat GetFormat() const { return m_Format; }
        uint32_t GetID() const { return m_ID; }
    };

} // namespace Engine

#endif // ENGINE_RHI_ITEXTURE
