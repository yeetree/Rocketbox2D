#include "RHI/Vulkan/RHI/VulkanPipeline.h"
#include "RHI/Vulkan/RHI/VulkanShader.h"
#include "Engine/Core/Assert.h"

namespace Engine
{

    vk::ShaderStageFlagBits VulkanPipeline::GetShaderStage(ShaderStage stage)
    {
        vk::ShaderStageFlagBits ss;
        switch(stage)
        {
            case ShaderStage::Vertex: ss = vk::ShaderStageFlagBits::eVertex; break;
            case ShaderStage::Fragment: ss = vk::ShaderStageFlagBits::eFragment; break;
        }
        return ss;
    }

    vk::PrimitiveTopology VulkanPipeline::GetPrimitiveTopology(PrimitiveTopology topology)
    {
        vk::PrimitiveTopology t;
        switch(topology)
        {
            case PrimitiveTopology::LineList: t = vk::PrimitiveTopology::eLineList; break;
            case PrimitiveTopology::PointList: t = vk::PrimitiveTopology::ePointList; break;
            case PrimitiveTopology::TriangleList: t = vk::PrimitiveTopology::eTriangleList; break;
        }
        return t;
    }

    vk::Format VulkanPipeline::GetVertexElementFormat(VertexElementType type)
    {
        vk::Format e;
        switch(type)
        {
            case VertexElementType::Int: e = vk::Format::eR32Sint; break;
            case VertexElementType::Float: e = vk::Format::eR32Sfloat; break;
            case VertexElementType::Vec2: e = vk::Format::eR32G32Sfloat; break;
            case VertexElementType::Vec3: e = vk::Format::eR32G32B32Sfloat; break;
            case VertexElementType::Vec4: e = vk::Format::eR32G32B32A32Sfloat; break;
        }
        return e;
    }

    VulkanPipeline::VulkanPipeline(VulkanContext* context, const PipelineDesc& desc)
    {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: VulkanPipeline(): context is nullptr!");
        ENGINE_CORE_ASSERT(desc.shader != nullptr, "Vulkan: VulkanPipeline(): desc.shader is nullptr!");
    
        // Get shader modules
        VulkanShader* vs = static_cast<VulkanShader*>(desc.shader);
        std::map<ShaderStage, VulkanShaderModule>& stages = vs->GetStages();

        // Get shader stage create infos
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos;
        for(auto const& [stage, module] : stages)
        {
            vk::PipelineShaderStageCreateInfo stageInfo;
            stageInfo.stage = GetShaderStage(stage);
            stageInfo.module = *module.shaderModule.get();
            stageInfo.pName = module.entryPoint.c_str();
            stageInfo.pSpecializationInfo = nullptr;
            shaderStageInfos.push_back(stageInfo);
        }

        // Vertex attributes

        // Binding desc
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        vk::VertexInputBindingDescription bindingDescription = { 0, desc.vertexLayout.GetStride(), vk::VertexInputRate::eVertex };
        
        // Attrib desc
        const std::vector<VertexElement>& elements = desc.vertexLayout.GetElements();
        for(int i = 0; i < elements.size(); i++) {
            vk::VertexInputAttributeDescription vdesc;
            vdesc.location = i;
            vdesc.binding = 0;
            vdesc.format = GetVertexElementFormat(elements[i].GetType());
            vdesc.offset = elements[i].GetOffset();
            attributeDescriptions.push_back(vdesc);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Topology
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            {},
            GetPrimitiveTopology(desc.topology),
            vk::False
        );

        // Viewport & Scissor dynamic states
        std::vector dynamicStates = {
            vk::DynamicState::eViewport,
		    vk::DynamicState::eScissor
        };
		vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // Blank VP and Scissor state
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;

        // TODO: Vulkan: Rasterizer options in PipelineDesc
        
        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = vk::False;
        rasterizer.lineWidth = 1.0f;

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.sampleShadingEnable = vk::False;
        
        // Blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        if(desc.blending)
        {
            colorBlendAttachment.blendEnable = vk::True;
            colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
            colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
        }
        else
        {
            colorBlendAttachment.blendEnable = vk::False;
        }

        vk::PipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.logicOpEnable = vk::False;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // Uniform bindings
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
        for(auto const& ub : desc.uniformBindings)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.binding = ub.binding;
            binding.descriptorType = vk::DescriptorType::eUniformBufferDynamic;
            binding.descriptorCount = 1;
            binding.stageFlags = GetShaderStage(ub.stage);
            layoutBindings.push_back(binding);
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();

        m_DescriptorSetLayout = vk::raii::DescriptorSetLayout(context->GetDevice(), layoutInfo);

        // Pipeline layout
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setLayoutCount         = 1;
        pipelineLayoutInfo.pSetLayouts            = &(*m_DescriptorSetLayout);
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        m_Layout = vk::raii::PipelineLayout(context->GetDevice(), pipelineLayoutInfo);
        
        vk::GraphicsPipelineCreateInfo gfxPipeInfo;
        gfxPipeInfo.stageCount = shaderStageInfos.size();
        gfxPipeInfo.pStages = shaderStageInfos.data();
        gfxPipeInfo.pVertexInputState = &vertexInputInfo;
        gfxPipeInfo.pInputAssemblyState = &inputAssembly;
        gfxPipeInfo.pViewportState = &viewportState;
        gfxPipeInfo.pRasterizationState = &rasterizer;
        gfxPipeInfo.pMultisampleState = &multisampling;
        gfxPipeInfo.pColorBlendState = &colorBlending;
        gfxPipeInfo.pDynamicState = &dynamicState;
        gfxPipeInfo.layout = m_Layout;
        gfxPipeInfo.renderPass = nullptr;

        vk::Format colorFormat = vk::Format::eR8G8B8A8Srgb; // TODO: Vulkan: Do not guess here 

        vk::PipelineRenderingCreateInfo pipeRenderInfo;
        pipeRenderInfo.colorAttachmentCount = 1;
        pipeRenderInfo.pColorAttachmentFormats = &colorFormat;

        vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain(
            gfxPipeInfo,
            pipeRenderInfo
        );

        m_Pipeline = vk::raii::Pipeline(context->GetDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
    };
} // namespace Engine
