#ifndef ENGINE_RENDERER_MESH
#define ENGINE_RENDERER_MESH

#include "engine_export.h"

#include <memory>
#include <cstdint>

#include "Engine/Renderer/RHI/IVertexArray.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine
{
    class ENGINE_EXPORT Mesh {
    public:
        Mesh() = default;
        Mesh(std::shared_ptr<IVertexArray> vao, uint32_t indexCount, VertexLayout layout) : m_VAO(vao), m_IndexCount(indexCount), m_Layout(layout) {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        };

        void Bind() {
            m_VAO->Bind();
        }

        void Unbind() {
            m_VAO->Unbind();
        }

        uint32_t GetIndexCount() const { return m_IndexCount; }
        const VertexLayout& GetLayout() const { return m_Layout; }
        uint32_t GetID() const { return m_ID; }

    private:
        std::shared_ptr<IVertexArray> m_VAO;
        uint32_t m_IndexCount;
        VertexLayout m_Layout;
        uint32_t m_ID;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_MESH
