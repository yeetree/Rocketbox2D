#ifndef ENGINE_RENDERER_MESH
#define ENGINE_RENDERER_MESH

#include "engine_export.h"

#include <memory>
#include <cstdint>

#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine
{
    class ENGINE_EXPORT Mesh {
    public:
        Mesh() = default;
        Mesh(std::shared_ptr<IBuffer> vbo, std::shared_ptr<IBuffer> ebo, uint32_t indexCount, VertexLayout layout) : m_VBO(vbo), m_EBO(ebo), m_IndexCount(indexCount), m_Layout(layout) {
            static uint32_t nextID = 0;
            m_ID = nextID++;
        };

        void Bind() {
            m_VBO->Bind();
            m_EBO->Bind();
        }

        uint32_t GetIndexCount() const { return m_IndexCount; }
        const VertexLayout& GetLayout() const { return m_Layout; }
        uint32_t GetID() const { return m_ID; }

    private:
        std::shared_ptr<IBuffer> m_VBO, m_EBO;
        uint32_t m_IndexCount;
        VertexLayout m_Layout;
        uint32_t m_ID;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_MESH
