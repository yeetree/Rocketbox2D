#ifndef ENGINE_RENDERER_RHI_IBUFFER
#define ENGINE_RENDERER_RHI_IBUFFER

#include "engine_export.h"

#include <cstddef>
#include <cstdint>

namespace Engine {
    // BufferType: What kind of data the buffer stores
    enum class BufferType { Vertex, Index, Uniform };
    // NOTE: Uniform BufferType is used internally to create them.
    // For usage, use IUniformBuffer or platform eqiv

    // BufferDesc: Describes how a buffer should be created
    struct BufferDesc {
        size_t size = 0;                        // Size of buffer data in bytes
        BufferType type = BufferType::Vertex;   // Type of buffer
        const void* data = nullptr;             // Initial data (optional)
        bool isDynamic = false;                 // If the data is static or not
    };

    // Buffer
    class ENGINE_EXPORT IBuffer {
    public:
        IBuffer() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IBuffer() = default;

        uint32_t GetID() const { return m_ID; }

        virtual void UpdateData(const void* data, size_t size, size_t offset) = 0;
    private:
        uint32_t m_ID;
    };

} // namespace Engine


#endif // ENGINE_RENDERER_RHI_IBUFFER
