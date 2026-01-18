#ifndef ENGINE_RENDERER_IPIPELINESTATE
#define ENGINE_RENDERER_IPIPELINESTATE

#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine {
    // How the triangles should be rendered
    enum class FillMode { Solid, Wireframe };

    // How the triangles should be culled
    enum class CullMode { None, Front, Back };

    // Describes how a Pipeline State Object should be created
    struct PipelineDesc {
        IShader* shader = nullptr;  // Shader to be used with pipeline
        VertexLayout layout;        // Vertex attributes
        
        // Rasterizer State
        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::Back;
        
        // Blend State
        bool enableBlending = false;
        
        // Depth/Stencil State
        bool depthTest = true;
        bool depthWrite = true;
    };

    // Pipeline State Object
    class IPipelineState {
    public:
        IPipelineState() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IPipelineState() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        uint32_t GetID() const { return m_ID; }

    private:
        uint32_t m_ID;
    };
}

#endif // ENGINE_RENDERER_IPIPELINESTATE
