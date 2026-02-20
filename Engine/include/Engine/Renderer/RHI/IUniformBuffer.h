#ifndef ENGINE_RENDERER_RHI_IUNIFORMBUFFER
#define ENGINE_RENDERER_RHI_IUNIFORMBUFFER

#include "engine_export.h"

#include <cstddef>
#include <cstdint>

namespace Engine
{
    // UniformBufferDesc: Describes how a uniform buffer should be created
    struct UniformBufferDesc {
        size_t size = 0;            // Size of buffer data in bytes
        const void* data = nullptr; // Initial data (optional)
    };

    // Very similar to IBuffer, but separate object for simplicity
    // (Vulkan is holding me at gunpoint)
    class ENGINE_EXPORT IUniformBuffer {
    public:
        IUniformBuffer() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IUniformBuffer() = default;

        uint32_t GetID() const { return m_ID; }

        virtual void UpdateData(const void* data, size_t size, size_t offset) = 0;
    private:
        uint32_t m_ID;
    };
    
} // namespace Engine

#endif // ENGINE_RENDERER_RHI_IUNIFORMBUFFER
