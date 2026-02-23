#ifndef ENGINE_RENDERER_ITEXTURE
#define ENGINE_RENDERER_ITEXTURE

#include "engine_export.h"

#include <cstdint>

namespace Engine {
    // Format of texture data
    enum class TextureFormat { RGBA8 };

    // Texture usage
    enum class TextureUsage { Sampled };

    // Decribes how a texture should be created
    struct TextureDesc {
        uint32_t width = 0;
        uint32_t height = 0;
        // uint32_t mips = 1;
        TextureFormat format = TextureFormat::RGBA8;
        TextureUsage usage = TextureUsage::Sampled;
        const void* data = nullptr;
    };

    // Texture
    class ENGINE_EXPORT ITexture {
    public:
        ITexture() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~ITexture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;

        uint32_t GetID() const { return m_ID; }
    private:
        uint32_t m_ID;
    };

} // namespace Engine


#endif // ENGINE_RENDERER_ITEXTURE
