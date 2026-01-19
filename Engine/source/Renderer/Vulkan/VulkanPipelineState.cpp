#include "Renderer/Vulkan/VulkanPipelineState.h"
#include "Renderer/Vulkan/VulkanShader.h"

namespace Engine
{
    VulkanPipelineState::VulkanPipelineState(const vk::raii::Device& device, const vk::Format& colorAttachmentFormat, const PipelineDesc& desc) {
        // Get Vulkan Shader
        VulkanShader* shader = static_cast<VulkanShader*>(desc.shader);

        // Get shader stages and create PipelineShaderStages
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        for(auto [stage, blob] : shader->m_Stages) {
            // Blob: Pair: std::string, std::shared_ptr<vk::raii::ShaderModule>
            vk::PipelineShaderStageCreateInfo shaderStageInfo;
            shaderStageInfo.stage = GetVulkanShaderStage(stage);
            shaderStageInfo.module = *blob.second;
            shaderStageInfo.pName = blob.first;
            stages.push_back(shaderStageInfo);
        }

        // Hardcoding for now...
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer;
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = vk::False;
        rasterizer.depthBiasSlopeFactor = 1.0f;
        rasterizer.lineWidth = 1.0f;

		vk::PipelineMultisampleStateCreateInfo multisampling;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.sampleShadingEnable = vk::False;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.blendEnable = vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

		vk::PipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.logicOpEnable = vk::False;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // Dynamic rendering
		std::vector dynamicStates = {
            vk::DynamicState::eViewport,
		    vk::DynamicState::eScissor
        };

		vk::PipelineDynamicStateCreateInfo dynamicState;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // Create pipelime
        vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo; 
        pipelineRenderingCreateInfo.colorAttachmentCount = 1;
        pipelineRenderingCreateInfo.pColorAttachmentFormats = &colorAttachmentFormat;
        
        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.pNext = &pipelineRenderingCreateInfo;
        pipelineInfo.stageCount = stages.size();
        pipelineInfo.pStages = stages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly,
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

    vk::ShaderStageFlagBits VulkanPipelineState::GetVulkanShaderStage(ShaderStage stage) {
        switch(stage) {
            case ShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex; break;
            case ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment; break;
            case ShaderStage::Geometry: return vk::ShaderStageFlagBits::eGeometry; break;
        }
        return vk::ShaderStageFlagBits::eVertex; // Should never really happen
    }
} // namespace Engine
