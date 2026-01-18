#ifndef ENGINE_RENDERER_RHI_IVERTEXARRAY
#define ENGINE_RENDERER_RHI_IVERTEXARRAY

#include <cstdint>
#include <memory>
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine
{
    // Decribes how a vertex array should be created
    struct VertexArrayDesc {
        std::shared_ptr<IBuffer> vbo;
        std::shared_ptr<IBuffer> ebo;
        VertexLayout layout;
    };

    class IVertexArray {
    public:
        IVertexArray() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IVertexArray() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

    private:
        uint32_t m_ID;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_RHI_IVERTEXARRAY
