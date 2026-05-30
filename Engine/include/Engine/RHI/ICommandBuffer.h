#ifndef ENGINE_RHI_ICOMMANDBUFFER
#define ENGINE_RHI_ICOMMANDBUFFER

#include "engine_export.h"

#include "Engine/RHI/RHI.h"

#include "Engine/Math/Vector.h"

namespace Engine::RHI
{
    class ENGINE_EXPORT ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        // Graphics
        virtual void BindPipeline(PipelineHandle pipeline) = 0;
        virtual void BindVertexBuffer(BufferHandle buffer) = 0;
        virtual void BindIndexBuffer(BufferHandle buffer) = 0;
        virtual void BindUniformBuffer(BufferHandle buffer, uint32_t binding) = 0;
        virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t indexOffset = 0, uint32_t firstInstance = 0) = 0;
   
        // Data
        // offset is only used by static buffers and is ignored by dynamic buffers.
        virtual void UploadBuffer(BufferHandle buffer, void* data, size_t size, size_t offset) = 0;
        virtual void UploadTexture(TextureHandle texture, void* data) = 0;
    };
}

#endif // ENGINE_RHI_ICOMMANDBUFFER
