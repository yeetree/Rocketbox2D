#ifndef RENDERER_VULKAN_VULKANSHADER
#define RENDERER_VULKAN_VULKANSHADER

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IShader.h"

namespace Engine {
    class VulkanGraphicsDevice;

    struct VulkanShader : public IShader {
    public:
        VulkanShader(VulkanGraphicsDevice* graphicsDevice, const ShaderDesc& desc);
        ~VulkanShader() override;

        // Stage to name + module
        std::map<ShaderStage, std::pair<std::string, std::shared_ptr<vk::raii::ShaderModule>>> m_Stages;
        
        // Layout
        vk::raii::PipelineLayout m_Layout = nullptr;

    private:
        // Creates shader module from bytecode
        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const vk::raii::Device& device, const std::vector<char>& code) const;
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANSHADER
