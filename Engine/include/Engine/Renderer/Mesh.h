#ifndef ENGINE_RENDERER_MESH
#define ENGINE_RENDERER_MESH

#include "engine_export.h"

#include <memory>
#include <cstdint>

#include "Engine/Renderer/RHI/IBuffer.h"

namespace Engine
{
    class ENGINE_EXPORT Mesh {
    public:
        Mesh(std::shared_ptr<IBuffer> vbo, std::shared_ptr<IBuffer> ebo, uint32_t indexCount) : m_VBO(vbo), m_EBO(ebo), m_IndexCount(indexCount) {};

        void Bind() {
            m_VBO->Bind();
            m_EBO->Bind();
        }

        uint32_t GetIndexCount() {
            return m_IndexCount;
        }

    private:
        std::shared_ptr<IBuffer> m_VBO, m_EBO;
        uint32_t m_IndexCount;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_MESH
