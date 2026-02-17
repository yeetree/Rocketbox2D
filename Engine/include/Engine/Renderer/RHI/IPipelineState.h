#ifndef ENGINE_RENDERER_IPIPELINESTATE
#define ENGINE_RENDERER_IPIPELINESTATE

#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

namespace Engine {

    // Formats?
    enum class Format { Undefined, RGBA8, RGB8, RGBA16F, Depth24Stencil8 };

    // What are the shapes?
    enum class PrimitiveTopology { PointList, LineList, TriangleList };

    // How the shapes should be rendered
    enum class FillMode { Solid, Wireframe };

    // How the shapes should be culled
    enum class CullMode { None, Front, Back };

    // Describes how a Pipeline State Object should be created
    struct PipelineDesc {
        IShader* shader = nullptr;  // Shader to be used with pipeline
        VertexLayout layout;        // Vertex attributes

        // Uniform buffers and push constants
        uint32_t numUniformBuffers;
        uint32_t pushConstantSize;
        
        // Topology
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

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

        uint32_t GetID() const { return m_ID; }

    private:
        uint32_t m_ID;
    };
}

#endif // ENGINE_RENDERER_IPIPELINESTATE
