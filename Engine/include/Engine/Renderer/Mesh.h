#ifndef ENGINE_RENDERER_MESH
#define ENGINE_RENDERER_MESH

#include "engine_export.h"

#include <memory>
#include <cstdint>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine
{
    class ENGINE_EXPORT Mesh {
    public:
        Mesh() = default;
        Mesh(Ref<IBuffer> vertexBuffer, Ref<IBuffer> indexBuffer, uint32_t indexCount, VertexLayout layout)
            : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_IndexCount(indexCount), m_Layout(layout)
        {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        };

        Ref<IBuffer> GetVertexBuffer() { return m_VertexBuffer; }
        Ref<IBuffer> GetIndexBuffer() { return m_IndexBuffer; }
        uint32_t GetIndexCount() const { return m_IndexCount; }
        const VertexLayout& GetLayout() const { return m_Layout; }
        uint32_t GetID() const { return m_ID; }

    private:
        Ref<IBuffer> m_VertexBuffer, m_IndexBuffer;
        uint32_t m_IndexCount;
        VertexLayout m_Layout;
        uint32_t m_ID;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_MESH
