#include "RHI/Vulkan/RHI/VulkanShader.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    [[nodiscard]] vk::raii::ShaderModule VulkanShader::CreateShaderModule(const vk::raii::Device& device, const std::vector<uint32_t>& code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();
        return vk::raii::ShaderModule{ device, createInfo };
    }

    VulkanShader::VulkanShader(VulkanContext* context, const ShaderDesc& desc) {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: VulkanShader(): context is nullptr!");

        // Loop through all shader modules
        for (const ShaderModule& mod : desc.modules) {
            // Create a shared pointer to this specific module
            auto modulePtr = CreateRef<vk::raii::ShaderModule>(
                std::move(CreateShaderModule(context->GetDevice(), mod.byteCode))
            );

            // Loop through stages and store a reference to the shader module
            for(auto const &[stage, entryPoint] : mod.entryPoints) {
                m_Stages[stage] = { entryPoint, modulePtr };
            }
        }
    }

} // namespace Engine
