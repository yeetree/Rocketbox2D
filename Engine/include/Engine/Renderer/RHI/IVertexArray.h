#ifndef ENGINE_RENDERER_RHI_IVERTEXARRAY
#define ENGINE_RENDERER_RHI_IVERTEXARRAY

#include "engine_export.h"

#include <cstdint>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine
{
    // Decribes how a vertex array should be created
    struct VertexArrayDesc {
        Ref<IBuffer> vbo;
        Ref<IBuffer> ebo;
        VertexLayout layout;
    };

    class ENGINE_EXPORT IVertexArray {
    public:
        IVertexArray() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IVertexArray() = default;

    private:
        uint32_t m_ID;
    };
} // namespace Engine

#endif // ENGINE_RENDERER_RHI_IVERTEXARRAY
