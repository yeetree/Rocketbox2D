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
        rasterizer.frontFace = vk::FrontFace::eClockwise;
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
        std::vector<vk::DescriptorSetLayout> setLayouts;

        const std::vector<ShaderBinding>& bindings = desc.shaderLayout.GetBindings();

        // group bindings by set and create descriptor bindings
        std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> sets;
        for (const ShaderBinding& shaderBinding : bindings) {
            vk::DescriptorSetLayoutBinding descriptorBinding{};
            descriptorBinding.binding = shaderBinding.binding;
            descriptorBinding.descriptorCount = 1;

            switch (shaderBinding.type) {
                case ShaderBindingType::UniformBuffer:
                    descriptorBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
                    // Best practice: allow UBOs in both vertex and fragment stages
                    descriptorBinding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
                    break;
                case ShaderBindingType::Sampler:
                    descriptorBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                    descriptorBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
                    break;
            }
            

            m_SlotToSetBindingMap[shaderBinding.slot] = { shaderBinding.set, shaderBinding.binding };

            // Add binding to set
            sets[shaderBinding.set].push_back(descriptorBinding);
        }

        // create descriptor sets
        for (auto const& [setIndex, bindingsList] : sets) {
            vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
            descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindingsList.size());
            descriptorSetLayoutInfo.pBindings = bindingsList.data();

            // Store the layout as well
            m_DescriptorSetLayouts.emplace(setIndex, vk::raii::DescriptorSetLayout(
                m_GraphicsDevice->GetDevice().GetDevice(), 
                descriptorSetLayoutInfo
            ));
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

        std::vector<vk::DescriptorSetLayout> rawLayouts;
        rawLayouts.reserve(m_DescriptorSetLayouts.size());

        for (const auto& layout : m_DescriptorSetLayouts) {
            rawLayouts.push_back(*layout.second);
        }

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

    vk::DescriptorSet VulkanPipelineState::GetDescriptorSetForTexture(VulkanTexture& tex, uint32_t slot) {
        ResourceKey key = { tex.GetID(), slot, 0};
    
        if (m_DescriptorCacheTextures.find(key) == m_DescriptorCacheTextures.end()) {
            // Get set from slot
            SetBinding setBinding = m_SlotToSetBindingMap[slot];
            const vk::DescriptorSetLayout& layout = *m_DescriptorSetLayouts.at(setBinding.set);

            // allocate set
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.descriptorPool = *m_GraphicsDevice->GetDescriptorPool();
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &layout;

            std::vector<vk::raii::DescriptorSet> sets = m_GraphicsDevice->GetDevice().GetDevice().allocateDescriptorSets(allocInfo);
            
            vk::raii::DescriptorSet descriptorSet = std::move(sets[0]);

            // update the Set
            vk::DescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.imageView = *tex.GetImageView();
            imageInfo.sampler = *tex.GetSampler();

            vk::WriteDescriptorSet write{};
            write.dstSet = *descriptorSet;
            write.dstBinding = setBinding.binding;
            write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfo;

            m_GraphicsDevice->GetDevice().GetDevice().updateDescriptorSets(write, nullptr);

            m_DescriptorCacheTextures.emplace(key, std::move(descriptorSet));
        }
        
        return *m_DescriptorCacheTextures.at(key);
    }

    vk::DescriptorSet VulkanPipelineState::GetDescriptorSetForUniformBuffer(VulkanUniformBuffer& ubo, uint32_t slot) {
        uint32_t frameIndex = m_GraphicsDevice->GetFrameIndex();
        ResourceKey key = { ubo.GetID(), slot, frameIndex };

        if (m_DescriptorCacheUniformBufferObjects.find(key) == m_DescriptorCacheUniformBufferObjects.end()) {
            SetBinding setBinding = m_SlotToSetBindingMap[slot];
            const vk::DescriptorSetLayout& layout = *m_DescriptorSetLayouts.at(setBinding.set);

            // Allocate Set
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.descriptorPool = *m_GraphicsDevice->GetDescriptorPool();
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &layout;

            auto sets = m_GraphicsDevice->GetDevice().GetDevice().allocateDescriptorSets(allocInfo);
            vk::raii::DescriptorSet descriptorSet = std::move(sets[0]);

            // Update the Set for a Buffer
            vk::DescriptorBufferInfo bufferInfo{};
            // GetVulkanBuffer() should return the specific VkBuffer for this frame
            bufferInfo.buffer = ubo.GetBuffer(frameIndex).GetBuffer(); 
            bufferInfo.offset = 0;
            bufferInfo.range = ubo.GetSize();

            vk::WriteDescriptorSet write{};
            write.dstSet = *descriptorSet;
            write.dstBinding = setBinding.binding;
            write.descriptorType = vk::DescriptorType::eUniformBuffer;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            m_GraphicsDevice->GetDevice().GetDevice().updateDescriptorSets(write, nullptr);

            m_DescriptorCacheUniformBufferObjects.emplace(key, std::move(descriptorSet));
        }

        return *m_DescriptorCacheUniformBufferObjects.at(key);
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
} // namespace Engine
