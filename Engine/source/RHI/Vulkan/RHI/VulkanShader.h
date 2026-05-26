#ifndef RHI_VULKAN_RHI_VULKANSHADER
#define RHI_VULKAN_RHI_VULKANSHADER

#include "Engine/RHI/IShader.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    struct ENGINE_EXPORT VulkanShaderModule
    {
        std::string entryPoint;
        Ref<vk::raii::ShaderModule> shaderModule;
    };

    class ENGINE_EXPORT VulkanShader : public IShader
    {
    private:
        std::map<ShaderStage, VulkanShaderModule> m_Stages;

        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const vk::raii::Device& device, const std::vector<uint32_t>& code) const;

    public:
        VulkanShader(VulkanContext* context, const ShaderDesc& desc);
        ~VulkanShader() override = default;

        std::map<ShaderStage, VulkanShaderModule>& GetStages() { return m_Stages; }
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANSHADER
