#include "Renderer/Vulkan/RHI/VulkanShader.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanShader::VulkanShader(VulkanGraphicsDevice* graphicsDevice, const ShaderDesc& desc) {
        ENGINE_CORE_ASSERT(graphicsDevice != nullptr, "Vulkan: invalid graphics device when creating shader!");

        // Loop through all shader modules
        for (const ShaderModule& mod : desc.modules) {
            // Create a shared pointer to this specific module
            auto modulePtr = std::make_shared<vk::raii::ShaderModule>(
                std::move(CreateShaderModule(graphicsDevice->GetDevice().GetDevice(), mod.byteCode))
            );

            // Loop through all stages and store the module pointer by the entrypoint name
            for(auto const &[stage, entryPoint] : mod.entryPoints) {
                m_Stages[stage] = { entryPoint, modulePtr };
            }
        }
    }

    VulkanShader::~VulkanShader() {
        
    }

    std::map<ShaderStage, std::pair<std::string, std::shared_ptr<vk::raii::ShaderModule>>>& VulkanShader::GetStages() {
        return m_Stages;
    }

    [[nodiscard]] vk::raii::ShaderModule VulkanShader::CreateShaderModule(const vk::raii::Device& device, const std::vector<uint32_t>& code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();
        return vk::raii::ShaderModule{ device, createInfo };
    }
} // namespace Engine
