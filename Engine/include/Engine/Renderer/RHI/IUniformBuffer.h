#ifndef ENGINE_RENDERER_RHI_IUNIFORMBUFFER
#define ENGINE_RENDERER_RHI_IUNIFORMBUFFER

#include <cstddef>
#include <cstdint>

namespace Engine
{
    // UniformBufferDesc: Describes how a uniform buffer should be created
    struct UniformBufferDesc {
        size_t size;                // Size of buffer data in bytes
        const void* data = nullptr; // Initial data (optional)
    };

    // Very similar to IBuffer, but separate object for simplicity
    // (Vulkan is holding me at gunpoint)
    class IUniformBuffer {
    public:
        virtual ~IUniformBuffer() = default;

        virtual void UpdateData(const void* data, size_t size, size_t offset) = 0;
    };
    
} // namespace Engine

#endif // ENGINE_RENDERER_RHI_IUNIFORMBUFFER
