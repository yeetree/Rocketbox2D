#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanGraphicsDevice.h"

namespace Engine
{
    VulkanShader::VulkanShader(VulkanGraphicsDevice* graphicsDevice, const ShaderDesc& desc) {
        
        // Key by the bytecode data to find duplicates within this Desc
        std::map<std::vector<char>, std::shared_ptr<vk::raii::ShaderModule>> uniqueModules;

        for (auto const& [stage, blob] : desc.stages) {
            if (blob.byteCode.empty()) continue;

            auto it = uniqueModules.find(blob.byteCode);
            std::shared_ptr<vk::raii::ShaderModule> modulePtr;

            if (it == uniqueModules.end()) {
                modulePtr = std::make_shared<vk::raii::ShaderModule>(CreateShaderModule(graphicsDevice->m_Device->GetDevice(), blob.byteCode));
                uniqueModules[blob.byteCode] = modulePtr;
            } else {
                modulePtr = it->second;
            }

            // Store stage info: entryPoint & modulePtr
            m_Stages[stage] = { blob.entryPoint, modulePtr };
        }

        // Create pipeline layout
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
		m_Layout = vk::raii::PipelineLayout(graphicsDevice->m_Device->GetDevice(), pipelineLayoutInfo);
    }

    VulkanShader::~VulkanShader() {
        
    }

    [[nodiscard]] vk::raii::ShaderModule VulkanShader::CreateShaderModule(const vk::raii::Device& device, const std::vector<char>& code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(char);
        // Iffy
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        return vk::raii::ShaderModule{ device, createInfo };
    }
} // namespace Engine
