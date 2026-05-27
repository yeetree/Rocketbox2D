#ifndef ENGINE_RHI_IBUFFER
#define ENGINE_RHI_IBUFFER

#include "engine_export.h"

#include <cstddef>
#include <cstdint>

namespace Engine
{
    // Forward:
    class ICommandBuffer;

    enum class ENGINE_EXPORT BufferType
    {
        Vertex,
        Index
    };

    enum class ENGINE_EXPORT BufferUsage
    {
        Static, // Updated rarely
        Dynamic // MUST be updated EVERY FRAME
    };

    struct ENGINE_EXPORT BufferDesc
    {
        size_t size = 0; // Size of buffer data in bytes
        BufferType type = BufferType::Vertex;
        BufferUsage usage = BufferUsage::Static;
    };
    
    class ENGINE_EXPORT IBuffer
    {
    private:
        uint32_t m_ID;
        size_t m_Size;
        BufferType m_Type;
        BufferUsage m_Usage;

    protected:
        IBuffer(size_t size, BufferType type, BufferUsage usage)
            : m_Size(size), m_Type(type), m_Usage(usage)
        {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

    public:
        virtual ~IBuffer() = default;

        // Getters
        size_t GetSize() const { return m_Size; }
        BufferType GetType() const { return m_Type; }
        BufferUsage GetUsage() const { return m_Usage; }
        uint32_t GetID() const { return m_ID; }
    };

} // namespace Engine


#endif // ENGINE_RHI_IBUFFER
