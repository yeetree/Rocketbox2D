#ifndef ENGINE_RHI_IPIPELINE
#define ENGINE_RHI_IPIPELINE

#include "engine_export.h"

#include "Engine/RHI/IShader.h"
#include "Engine/RHI/VertexLayout.h"

#include <cstdint>

namespace Engine
{
    struct ENGINE_EXPORT UniformBinding
    {
        uint32_t binding;
        ShaderStage stage;
    };

    enum class ENGINE_EXPORT PrimitiveTopology
    {
        PointList,
        LineList,
        TriangleList
    };

    struct ENGINE_EXPORT PipelineDesc
    {
        // Shader
        IShader* shader;

        // Vertex attributes
        VertexLayout vertexLayout;

        // Uniforms
        std::vector<UniformBinding> uniformBindings;

        // Topology
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

        // Rasterization
        bool blending = false;
    };

    class ENGINE_EXPORT IPipeline
    {
    private:
        uint32_t m_ID;

    protected:
        IPipeline() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

    public:
        virtual ~IPipeline() = default;
        
        uint32_t GetID() const { return m_ID; }
    };
} // namespace Engine


#endif // ENGINE_RHI_IPIPELINE
