#ifndef RENDERER_VULKAN_VULKANSHADER
#define RENDERER_VULKAN_VULKANSHADER

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IShader.h"

namespace Engine {
    class VulkanGraphicsDevice;

    class VulkanShader : public IShader {
    public:
        VulkanShader(VulkanGraphicsDevice* graphicsDevice, const ShaderDesc& desc);
        ~VulkanShader() override;

        // Public getters for VulkanPipelineState
        std::map<ShaderStage, std::pair<std::string, std::shared_ptr<vk::raii::ShaderModule>>>& GetStages();

    private:
        // Stage to name + module
        std::map<ShaderStage, std::pair<std::string, std::shared_ptr<vk::raii::ShaderModule>>> m_Stages;

        // Creates shader module from bytecode
        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const vk::raii::Device& device, const std::vector<uint32_t>& code) const;
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANSHADER
