#ifndef ENGINE_RENDERER_IPIPELINESTATE
#define ENGINE_RENDERER_IPIPELINESTATE

#include "Engine/Renderer/IShader.h"
#include "Engine/Renderer/VertexLayout.h"

namespace Engine {
    enum class FillMode { Solid, Wireframe };
    enum class CullMode { None, Front, Back };

    struct PipelineDesc {
        IShader* shader = nullptr;
        VertexLayout layout;
        
        // Rasterizer State
        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::Back;
        
        // Blend State
        bool enableBlending = false;
        
        // Depth/Stencil State
        bool depthTest = true;
        bool depthWrite = true;
    };

    class IPipelineState {
    public:
        virtual ~IPipelineState() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };
}

#endif // ENGINE_RENDERER_IPIPELINESTATE
