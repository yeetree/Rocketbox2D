#include "Renderer/Vulkan/VulkanPipelineState.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    VulkanPipelineState::VulkanPipelineState(const vk::raii::Device& device, const vk::Format& colorAttachmentFormat, const PipelineDesc& desc) {
        // Get Vulkan Shader
        VulkanShader* shader = static_cast<VulkanShader*>(desc.shader);

        // Get shader stages and create PipelineShaderStages
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        for(auto const& [stage, blob] : shader->m_Stages) {
            // Blob: Pair: std::string, std::shared_ptr<vk::raii::ShaderModule>
            vk::PipelineShaderStageCreateInfo shaderStageInfo;
            shaderStageInfo.stage = GetVulkanShaderStage(stage);
            shaderStageInfo.module = *blob.second;
            shaderStageInfo.pName = blob.first.c_str();
            stages.push_back(shaderStageInfo);
        }

        // Vertex attribs
        // Binding desc
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        vk::VertexInputBindingDescription bindingDescription = { 0, desc.layout.GetStride(), vk::VertexInputRate::eVertex };
        
        // Attrib desc
        const std::vector<VertexElement>& elements = desc.layout.GetElements();
        for(int i = 0; i < elements.size(); i++) {
            attributeDescriptions.emplace_back(i, 0, GetVulkanFormat(elements[i].type), elements[i].offset);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = vk::False;
        rasterizer.depthBiasSlopeFactor = 1.0f;
        rasterizer.lineWidth = 1.0f;

		vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.sampleShadingEnable = vk::False;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

		vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.logicOpEnable = vk::False;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // Dynamic rendering
		std::vector dynamicStates = {
            vk::DynamicState::eViewport,
		    vk::DynamicState::eScissor
        };

		vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // Create pipelime
        vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{}; 
        pipelineRenderingCreateInfo.colorAttachmentCount = 1;
        pipelineRenderingCreateInfo.pColorAttachmentFormats = &colorAttachmentFormat;
        
        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.pNext = &pipelineRenderingCreateInfo;
        pipelineInfo.stageCount = stages.size();
        pipelineInfo.pStages = stages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = shader->m_Layout;
        pipelineInfo.renderPass = nullptr;

        m_Pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo);
    }

    VulkanPipelineState::~VulkanPipelineState() {

    }

    vk::Format VulkanPipelineState::GetVulkanFormat(VertexElementType type) {
        switch(type) {
            case VertexElementType::Float:  return vk::Format::eR32Sfloat; break;
            case VertexElementType::Vec2:   return vk::Format::eR32G32Sfloat; break;
            case VertexElementType::Vec3:   return vk::Format::eR32G32B32Sfloat; break;
            case VertexElementType::Vec4:   return vk::Format::eR32G32B32A32Sfloat; break;
            case VertexElementType::Int:    return vk::Format::eR32Sint; break;
        }
        return vk::Format::eR32Sfloat; // Should never really happen
    }

    vk::ShaderStageFlagBits VulkanPipelineState::GetVulkanShaderStage(ShaderStage stage) {
        switch(stage) {
            case ShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex; break;
            case ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment; break;
            case ShaderStage::Geometry: return vk::ShaderStageFlagBits::eGeometry; break;
        }
        return vk::ShaderStageFlagBits::eVertex; // Should never really happen
    }
} // namespace Engine
