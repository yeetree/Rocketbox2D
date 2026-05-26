#ifndef RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
#define RHI_VULKAN_RHI_VULKANCOMMANDBUFFER

#include "Engine/RHI/ICommandBuffer.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/RHI/VulkanTexture.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanCommandBuffer : public ICommandBuffer
    {
    private:
        VulkanTexture* m_CurrentRenderTarget = nullptr;

        vk::raii::CommandBuffer m_CommandBuffer = nullptr;    

    public:
        VulkanCommandBuffer(VulkanContext* context);
        ~VulkanCommandBuffer() = default;

        void Begin() override;
        void End() override;

        // Render target
        void BeginRendering(ITexture* renderTarget, Vec4 clearColor) override;
        void EndRendering() override;

        // Graphics
        void BindPipeline(IPipeline* pipeline) override;
        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

        // Vulkan
        vk::raii::CommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
