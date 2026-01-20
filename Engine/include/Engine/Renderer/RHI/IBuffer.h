#ifndef ENGINE_RENDERER_RHI_IBUFFER
#define ENGINE_RENDERER_RHI_IBUFFER

#include <cstddef>
#include <cstdint>

namespace Engine {
    // BufferType: What kind of data the buffer stores
    enum class BufferType { Vertex, Index, Uniform };

    // BufferDesc: Describes how a buffer should be created
    struct BufferDesc {
        size_t size;                // Size of buffer data in bytes
        BufferType type;            // Type of buffer
        const void* data = nullptr; // Initial data (optional)
        bool isDynamic = false;     // If the data is static or not
    };

    // Buffer
    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual void UpdateData(const void* data, size_t size, size_t offset) = 0;
    };

} // namespace Engine


#endif // ENGINE_RENDERER_RHI_IBUFFER
