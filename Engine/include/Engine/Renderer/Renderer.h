#ifndef _ENGINE_RENDERER_RENDERER
#define _ENGINE_RENDERER_RENDERER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/RHI.h"
#include "Engine/RHI/ICommandBuffer.h"

#include "Engine/Math/Vector.h"

namespace Engine
{
    class ENGINE_EXPORT Renderer
    {
    private:
        // Sprite rendering data
        RHI::BufferHandle m_SpriteVertices;
        RHI::BufferHandle m_SpriteIndices;
        RHI::BufferHandle m_SpriteUniform;
        RHI::ShaderHandle m_SpriteShader;
        RHI::PipelineHandle m_SpritePipeline;
        RHI::TextureHandle m_DepthBuffer;

        // RHI
        RHI::ICommandBuffer* m_CurrentCommandBuffer = nullptr;

        // Refs
        
    public:
        Renderer();
        ~Renderer();

        void Begin(RHI::SwapChainHandle sc);
        void DrawSprite(RHI::TextureHandle tex, Vec2 pos, Vec2 size, float rot);
        void End();
    };
} // namespace Engine


#endif // _ENGINE_RENDERER_RENDERER
