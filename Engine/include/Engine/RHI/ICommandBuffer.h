#ifndef ENGINE_RHI_ICOMMANDBUFFER
#define ENGINE_RHI_ICOMMANDBUFFER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/ITexture.h"
#include "Engine/RHI/IPipeline.h"
#include "Engine/RHI/IBuffer.h"

#include "Engine/Math/Vector.h"

namespace Engine
{
    class ENGINE_EXPORT ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        // Command buffer
        virtual void Begin() = 0;
        virtual void End() = 0;

        // Render target
        virtual void BeginRendering(ITexture* renderTarget, Vec4 clearColor) = 0;
        virtual void EndRendering(ITexture* renderTarget) = 0;

        // Graphics
        virtual void BindPipeline(IPipeline* pipeline) = 0;
        virtual void BindVertexBuffer(IBuffer* buffer) = 0;
        virtual void BindIndexBuffer(IBuffer* buffer) = 0;
        virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t indexOffset = 0, uint32_t firstInstance = 0) = 0;
   
        // Data
        virtual void SetBufferData(IBuffer* buffer, void* data, size_t size, size_t offset) = 0;
    };
}

#endif // ENGINE_RHI_ICOMMANDBUFFER
