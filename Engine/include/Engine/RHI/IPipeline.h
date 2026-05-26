#ifndef ENGINE_RHI_IPIPELINE
#define ENGINE_RHI_IPIPELINE

#include "engine_export.h"

#include "Engine/RHI/IShader.h"

#include <cstdint>

namespace Engine
{
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
