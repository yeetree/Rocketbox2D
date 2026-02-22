#include "Renderer/Vulkan/RHI/VulkanPipelineState.h"
#include "Renderer/Vulkan/RHI/VulkanShader.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Renderer/Vulkan/RHI/VulkanTexture.h"
#include "Renderer/Vulkan/RHI/VulkanUniformBuffer.h"
#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanPipelineState::VulkanPipelineState(VulkanGraphicsDevice* graphicsDevice, const PipelineDesc& desc) : m_GraphicsDevice(graphicsDevice) {
        ENGINE_CORE_ASSERT(m_GraphicsDevice != nullptr, "Vulkan: invalid graphics device when creating pipeline state!");

        // Get Vulkan Shader
        if(desc.shader == nullptr) {
            LOG_CORE_ERROR("Vulkan: Cannot create pipeline state with null shader!");
            return;
        }

        VulkanShader* shader = static_cast<VulkanShader*>(desc.shader);

        // Get shader stages and create PipelineShaderStages
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        for(auto const& [stage, blob] : shader->GetStages()) {
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
        vk::VertexInputBindingDescription bindingDescription = { 0, desc.vertexLayout.GetStride(), vk::VertexInputRate::eVertex };
        
        // Attrib desc
        const std::vector<VertexElement>& elements = desc.vertexLayout.GetElements();
        for(int i = 0; i < elements.size(); i++) {
            attributeDescriptions.emplace_back(i, 0, GetVulkanFormat(elements[i].type), elements[i].offset);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = GetVulkanTopology(desc.topology);
        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = GetVulkanFillMode(desc.fillMode);
        rasterizer.cullMode = GetVulkanCullMode(desc.cullMode);
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = vk::False;
        rasterizer.depthBiasSlopeFactor = 1.0f;
        rasterizer.lineWidth = 1.0f;

		vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.sampleShadingEnable = vk::False;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = desc.enableBlending ? vk::True : vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        // Color blending
        // (src * srcAlpha) + (dst * (1 - srcAlpha))
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

        // Alpha blending
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

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

        // Loop thru shader layout
        std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> setGroups;
        uint32_t maxSetIndex = 0;

        for (const ShaderBinding& sb : desc.shaderLayout.GetBindings()) {
            vk::DescriptorSetLayoutBinding dlb{};
            dlb.binding = sb.binding;
            dlb.descriptorCount = 1;
            dlb.stageFlags = (sb.type == ShaderBindingType::UniformBuffer) 
                ? (vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                : vk::ShaderStageFlagBits::eFragment;
            dlb.descriptorType = (sb.type == ShaderBindingType::UniformBuffer)
                ? vk::DescriptorType::eUniformBuffer : vk::DescriptorType::eCombinedImageSampler;

            setGroups[sb.set].push_back(dlb);
            maxSetIndex = std::max(maxSetIndex, sb.set);
        }

        // create layouts
        std::vector<vk::DescriptorSetLayout> rawLayouts;
        for (uint32_t i = 0; i <= maxSetIndex; i++) {
            if (setGroups.contains(i)) {
                vk::DescriptorSetLayoutCreateInfo layoutInfo({}, setGroups[i]);
                m_DescriptorSetLayouts.emplace(i, vk::raii::DescriptorSetLayout(m_GraphicsDevice->GetDevice().GetDevice(), layoutInfo));
            } else {
                // empty layout for skipped sets to keep alignment
                vk::DescriptorSetLayoutCreateInfo emptyInfo{};
                m_DescriptorSetLayouts.emplace(i, vk::raii::DescriptorSetLayout(m_GraphicsDevice->GetDevice().GetDevice(), emptyInfo));
            }
            rawLayouts.push_back(*m_DescriptorSetLayouts.at(i));
        }

        // Create pipelime
        vk::Format colorFormat = m_GraphicsDevice->GetSwapchain().GetSurfaceFormat().format;
        vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{}; 
        pipelineRenderingCreateInfo.colorAttachmentCount = 1;
        pipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;
        
        // Create pipeline layout

		// Push constants range
        vk::PushConstantRange pushRange;
        pushRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
        pushRange.offset = 0;
        pushRange.size = desc.pushConstantSize;

        vk::PipelineLayoutCreateInfo layoutInfo;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(rawLayouts.size());
        layoutInfo.pSetLayouts = rawLayouts.data();
        // Include pushConstants if size > 0
        layoutInfo.pushConstantRangeCount = (desc.pushConstantSize > 0) ? 1 : 0;
        layoutInfo.pPushConstantRanges = (desc.pushConstantSize > 0) ? &pushRange : nullptr;

        m_Layout = vk::raii::PipelineLayout(m_GraphicsDevice->GetDevice().GetDevice(), layoutInfo);

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
        pipelineInfo.layout = m_Layout;
        pipelineInfo.renderPass = nullptr;

        m_Pipeline = m_GraphicsDevice->GetDevice().GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
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

    vk::PrimitiveTopology VulkanPipelineState::GetVulkanTopology(PrimitiveTopology topology) {
        switch(topology) {
            case PrimitiveTopology::TriangleList:   return vk::PrimitiveTopology::eTriangleList; break;
            case PrimitiveTopology::LineList:       return vk::PrimitiveTopology::eLineList; break;
            case PrimitiveTopology::PointList:      return vk::PrimitiveTopology::ePointList; break;
        }
        return vk::PrimitiveTopology::eTriangleList; // Should never really happen
    }

    vk::PolygonMode VulkanPipelineState::GetVulkanFillMode(FillMode mode) {
        switch(mode) {
            case FillMode::Fill:   return vk::PolygonMode::eFill; break;
            case FillMode::Line:   return vk::PolygonMode::eLine; break;
        }
        return vk::PolygonMode::eFill; // Should never really happen
    }

    vk::CullModeFlagBits VulkanPipelineState::GetVulkanCullMode(CullMode mode) {
        switch(mode) {
            case CullMode::None:    return vk::CullModeFlagBits::eNone; break;
            case CullMode::Front:   return vk::CullModeFlagBits::eFront; break;
            case CullMode::Back:    return vk::CullModeFlagBits::eBack; break;
        }
        return vk::CullModeFlagBits::eFront; // Should never really happen
    }

    vk::raii::Pipeline& VulkanPipelineState::GetPipeline() {
        return m_Pipeline;
    }

    vk::raii::PipelineLayout& VulkanPipelineState::GetLayout() {
        return m_Layout;
    }

    std::map<uint32_t, vk::raii::DescriptorSetLayout>& VulkanPipelineState::GetDescriptorSetLayouts() {
        return m_DescriptorSetLayouts;
    }
    
} // namespace Engine
