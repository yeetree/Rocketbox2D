#ifndef ENGINE_RENDERER_IPIPELINESTATE
#define ENGINE_RENDERER_IPIPELINESTATE

#include "engine_export.h"

#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/VertexLayout.h"
#include "Engine/Renderer/RHI/ShaderLayout.h"

namespace Engine {
    // What are the shapes?
    enum class PrimitiveTopology { PointList, LineList, TriangleList };

    // How the shapes should be rendered
    enum class FillMode { Fill, Line };

    // How the shapes should be culled
    enum class CullMode { None, Front, Back };

    // Describes how a Pipeline State Object should be created
    struct PipelineDesc {
        IShader* shader = nullptr;  // Shader to be used with pipeline
        ShaderLayout shaderLayout;  // Info about uniforms / samplers / general reflection data
        size_t pushConstantSize = 0;
        
        VertexLayout vertexLayout;  // Vertex attributes (VertexLayout has an empty default constructor)
        
        // Topology
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

        // Rasterizer State
        FillMode fillMode = FillMode::Fill;
        CullMode cullMode = CullMode::Back;
        
        // Blend State
        bool enableBlending = false;
    };

    // Pipeline State Object
    class ENGINE_EXPORT IPipelineState {
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
