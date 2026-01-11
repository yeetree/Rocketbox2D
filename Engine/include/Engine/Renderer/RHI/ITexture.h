#ifndef ENGINE_RENDERER_ITEXTURE
#define ENGINE_RENDERER_ITEXTURE

#include "engine_export.h"

#include <cstdint>

namespace Engine {
    // Format of texture data
    enum class TextureFormat { RGBA8, RGB8, RGBA16F, Depth24Stencil8 };

    // Texture usage
    enum class TextureUsage { Sampled, RenderTarget, DepthStencil };

    // Decribes how a texture should be created
    struct TextureDesc {
        uint32_t width;
        uint32_t height;
        uint32_t mips = 1;
        TextureFormat format = TextureFormat::RGBA8;
        TextureUsage usage = TextureUsage::Sampled;
        const void* data = nullptr;
    };

    // Texture
    class ITexture {
    public:
        virtual ~ITexture() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;
        virtual void Bind(uint32_t slot = 0) = 0;
        virtual void Unbind(uint32_t slot = 0) = 0;
    };

} // namespace Engine


#endif // ENGINE_RENDERER_ITEXTURE
