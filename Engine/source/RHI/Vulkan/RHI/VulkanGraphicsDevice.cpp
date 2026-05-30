#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "RHI/Vulkan/VulkanConstants.h"
#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "Engine/Platform/IWindow.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

namespace Engine::RHI::Vulkan
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge)
        : m_Bridge(std::move(bridge)),
          m_Context(m_Bridge.get()),
          m_FrameIndex(0)
    {
        // Create frames
        for(uint32_t i = 0; i < k_MaxFramesInFlight; i++)
        {
            m_Frames.push_back(CreateScope<VulkanFrame>(m_Context));
        }

        // Immediate commands
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            m_Context.GetGraphicsQueue().familyIndex
        );
        m_ImmediatePool = vk::raii::CommandPool(m_Context.GetDevice(), poolInfo);
        m_ImmediateCommandBuffer = CreateScope<VulkanCommandBuffer>(*this, *m_ImmediatePool);

        // Immediate fence
        vk::FenceCreateInfo fenceInfo{};
        m_ImmediateFence = vk::raii::Fence(m_Context.GetDevice(), fenceInfo);
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        m_Context.GetDevice().waitIdle();

        // Destroy all buffers and textures because they are non-raii
        for(auto const& [id, data] : m_Buffers)
        {
            BufferHandle handle = {.id = id};
            DestroyBuffer(handle);
        }

        for(auto const& [id, data] : m_Textures)
        {
            TextureHandle handle = {.id = id};
            DestroyTexture(handle);
        }
    };

    // Resource creation
    BufferHandle VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc)
    {
        // Get data
        uint32_t id = AllocateID();
        VulkanBufferData& data = m_Buffers[id];
        data.desc = desc;
        
        // Dynamic buffers are allocated on the fly via VulkanDynamicBufferAllocator.
        // data.dynamicOffsets is already sized to k_MaxFramesInFlight.
        if(data.desc.usage == BufferUsage::Static)
        {
            // Buffer info
            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = data.desc.size;
            bufferInfo.usage = VulkanCommon::GetBufferUsageFlags(data.desc.type) | vk::BufferUsageFlagBits::eTransferDst;

            // Alloc info: map to GPU memory
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

            // Allocate / Create buffer
            VmaAllocationInfo resultInfo;
            VkBuffer buffer;
            vmaCreateBuffer(m_Context.GetAllocator(), bufferInfo, &allocInfo, &buffer, &data.allocation, &resultInfo);
            data.buffer = buffer;
        }

        return BufferHandle{ .id = id };
    }

    TextureHandle VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc)
    {
        // Get data
        uint32_t id = AllocateID();
        VulkanTextureData& data = m_Textures[id];
        data.desc = desc;

        // Create image
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = data.desc.width;
        imageInfo.extent.height = data.desc.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = static_cast<VkFormat>(VulkanCommon::GetSurfaceFormat(data.desc.format).format);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        
        VkImage image = nullptr;

        vmaCreateImage(
            m_Context.GetAllocator(), 
            &imageInfo, 
            &allocInfo, 
            &image, 
            &data.allocation, 
            nullptr
        );

        data.image = image;
        data.format = VulkanCommon::GetSurfaceFormat(data.desc.format).format;
        data.ownsImage = true;

        CreateImageView(data);

        if(data.desc.usage.Has(TextureUsage::Sampled))
        {
            CreateSampler(data);
        }

        return TextureHandle{ .id = id };
    }

    ShaderHandle VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc)
    {
        // Get data
        uint32_t id = AllocateID();
        VulkanShaderData& data = m_Shaders[id]; 

        // Loop through all shader modules
        for (const ShaderModule& mod : desc.modules) {
            // Add module and get index
            size_t moduleIndex = data.modules.size();
            data.modules.emplace_back(std::move(
                VulkanCommon::CreateShaderModule(m_Context.GetDevice(), mod.spirv)
            ));
            
            // Loop through stages and map stage to module index
            for(auto const &[stage, entryPoint] : mod.entryPoints) {
                data.stages[stage] = { moduleIndex, entryPoint };
            }
        }

        return ShaderHandle{ .id = id };
    }

    PipelineHandle VulkanGraphicsDevice::CreatePipeline(const PipelineDesc& desc)
    {
        // Get data
        uint32_t id = AllocateID();
        VulkanPipelineData& data = m_Pipelines[id]; 

        // Get shader data
        VulkanShaderData& shaderData = GetShaderData(desc.shader);

        // Get shader stage create infos
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos;
        for(auto const& [stage, info] : shaderData.stages)
        {
            vk::PipelineShaderStageCreateInfo stageInfo;
            stageInfo.stage = VulkanCommon::GetShaderStage(stage);
            stageInfo.module = shaderData.modules[info.moduleIndex];
            stageInfo.pName = info.entryPoint.c_str();
            stageInfo.pSpecializationInfo = nullptr;
            shaderStageInfos.push_back(stageInfo);
        }

        // Vertex attributes

        // Get vertex input attribute description
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        vk::VertexInputBindingDescription bindingDescription = { 0, desc.vertexLayout.GetStride(), vk::VertexInputRate::eVertex };
        const std::vector<VertexElement>& elements = desc.vertexLayout.GetElements();
        for(int i = 0; i < elements.size(); i++) {
            vk::VertexInputAttributeDescription vdesc;
            vdesc.location = i;
            vdesc.binding = 0;
            vdesc.format = VulkanCommon::GetVertexElementFormat(elements[i].GetType());
            vdesc.offset = elements[i].GetOffset();
            attributeDescriptions.push_back(vdesc);
        }

        // Get vertex input info
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Topology
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            {},
            VulkanCommon::GetPrimitiveTopology(desc.topology),
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
        
        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = VulkanCommon::GetPolygonMode(desc.polygonMode);
        rasterizer.cullMode = VulkanCommon::GetCullMode(desc.cullMode);
        rasterizer.frontFace = VulkanCommon::GetFrontFace(desc.frontFace);
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
            binding.descriptorType = VulkanCommon::GetUniformDescriptorType(ub.type);
            binding.descriptorCount = 1;
            binding.stageFlags = VulkanCommon::GetShaderStage(ub.stage);
            layoutBindings.push_back(binding);
        }

        // Descriptor set layout
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();
        data.descriptorSetLayout = vk::raii::DescriptorSetLayout(m_Context.GetDevice(), layoutInfo);

        // Pipeline layout
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setLayoutCount         = 1;
        pipelineLayoutInfo.pSetLayouts            = &(*data.descriptorSetLayout);
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        data.pipelineLayout = vk::raii::PipelineLayout(m_Context.GetDevice(), pipelineLayoutInfo);
        
        vk::GraphicsPipelineCreateInfo gfxPipeInfo;
        gfxPipeInfo.stageCount          = shaderStageInfos.size();
        gfxPipeInfo.pStages             = shaderStageInfos.data();
        gfxPipeInfo.pVertexInputState   = &vertexInputInfo;
        gfxPipeInfo.pInputAssemblyState = &inputAssembly;
        gfxPipeInfo.pViewportState      = &viewportState;
        gfxPipeInfo.pRasterizationState = &rasterizer;
        gfxPipeInfo.pMultisampleState   = &multisampling;
        gfxPipeInfo.pColorBlendState    = &colorBlending;
        gfxPipeInfo.pDynamicState       = &dynamicState;
        gfxPipeInfo.layout              = data.pipelineLayout;
        gfxPipeInfo.renderPass          = nullptr;

        // Get color attachment formats
        std::vector<vk::Format> colorAttachmentFormats;
        for(const PixelFormat& format : desc.colorAttachmentFormats)
        {
            colorAttachmentFormats.push_back(VulkanCommon::GetSurfaceFormat(format).format);
        }

        vk::PipelineRenderingCreateInfo pipeRenderInfo;
        pipeRenderInfo.colorAttachmentCount = colorAttachmentFormats.size();
        pipeRenderInfo.pColorAttachmentFormats = colorAttachmentFormats.data();

        vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain(
            gfxPipeInfo,
            pipeRenderInfo
        );

        data.pipeline = vk::raii::Pipeline(m_Context.GetDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());

        return PipelineHandle{ .id = id };
    }

    SwapChainHandle VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        ENGINE_CORE_ASSERT(desc.window != nullptr, "Vulkan: VulkanGraphicsDevice: CreateSwapChain(): desc.window is nullptr!");

        // Get data
        uint32_t id = AllocateID();
        VulkanSwapChainData& data = m_SwapChains[id]; 

        // Create surface
        vk::SurfaceKHR surface = m_Bridge->CreateSurface(
            m_Context.GetInstance(),
            m_Context.GetPhysicalDevice(),
            m_Context.GetGraphicsQueue().familyIndex,
            desc.window
        );
        data.surface = vk::raii::SurfaceKHR(m_Context.GetInstance(), surface);

        // Configuration
        data.format = desc.format;
        data.presentMode = desc.presentation;
        data.width = desc.window->GetWidth();
        data.height = desc.window->GetHeight();

        RebuildSwapchain(data);

        return SwapChainHandle{ .id = id };
    }

    // Resource destruction
    void VulkanGraphicsDevice::DestroyBuffer(BufferHandle& buffer)
    {
        VulkanBufferData& data = GetBufferData(buffer);
        if(data.desc.usage == BufferUsage::Static)
        {
            vmaDestroyBuffer(m_Context.GetAllocator(), data.buffer, data.allocation);
        }
        m_Buffers.erase(buffer.id);
        buffer.id = 0;
    }

    void VulkanGraphicsDevice::DestroyTexture(TextureHandle& texture)
    {
        VulkanTextureData& data = GetTextureData(texture);
        if(data.ownsImage && data.image && data.allocation)
        {
            vmaDestroyImage(m_Context.GetAllocator(), data.image, data.allocation);
        }
        m_Textures.erase(texture.id);
        texture.id = 0;
    }

    void VulkanGraphicsDevice::DestroyShader(ShaderHandle& shader)
    {
        VulkanShaderData& data = GetShaderData(shader);
        m_Shaders.erase(shader.id);
        shader.id = 0;
    }

    void VulkanGraphicsDevice::DestroyPipeline(PipelineHandle& pipeline)
    {
        VulkanPipelineData& data = GetPipelineData(pipeline);
        m_Pipelines.erase(pipeline.id);
        pipeline.id = 0;
    }

    void VulkanGraphicsDevice::DestroySwapChain(SwapChainHandle& swapchain)
    {
        VulkanSwapChainData& data = GetSwapChainData(swapchain);
        m_SwapChains.erase(swapchain.id);
        swapchain.id = 0;
    }

    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {
        // Wait for fence
        m_Context.GetDevice().waitForFences(*m_Frames[m_FrameIndex]->GetFence(), VK_TRUE, UINT64_MAX);
        m_Context.GetDevice().resetFences(*m_Frames[m_FrameIndex]->GetFence());
        m_Frames[m_FrameIndex]->Reset();

        // Clear previous submission info
        m_FrameSubmits.clear();
        m_FrameCommandBuffers.clear();
        m_FrameWaitSemaphores.clear();
        m_FrameSignalSemaphores.clear();
        m_FrameStageFlags.clear();
        m_FrameSwapChainPresentations.clear();
    }

    void VulkanGraphicsDevice::EndFrame()
    {
        // Submit everything
        m_Context.GetGraphicsQueue().queue.submit(
            m_FrameSubmits,
            *m_Frames[m_FrameIndex]->GetFence()
        );

        // Present all swapchains that were rendered to
        for (SwapChainHandle handle : m_FrameSwapChainPresentations)
        {
            VulkanSwapChainData& sc = GetSwapChainData(handle);

            vk::PresentInfoKHR presentInfo;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &*sc.renderFinishedSemaphores[sc.acquiredImageIndex];
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &*sc.swapchain;
            presentInfo.pImageIndices      = &sc.acquiredImageIndex;

            try
            {
                m_Context.GetGraphicsQueue().queue.presentKHR(presentInfo);
            }
            catch (const vk::OutOfDateKHRError&)
            {
                sc.needsRebuild = true;
            }
        }

        // Advance frame
        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    // Render passes
    //ICommandBuffer* VulkanGraphicsDevice::BeginPass(TextureHandle renderTarget, Vec4 clearColor)
    //{
    
    //}

    ICommandBuffer* VulkanGraphicsDevice::BeginPass(SwapChainHandle renderTarget, Vec4 clearColor)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(renderTarget);
        
        // Rebuild
        if(sc.needsRebuild)
        {
            RebuildSwapchain(sc);
        }

        // If we still need to rebuild, then we return nullptr
        if(sc.needsRebuild)
        {
            return nullptr;
        }

        // Acquire next image
        sc.acquiredImageIndex = sc.swapchain.acquireNextImage(
            UINT64_MAX,
            sc.presentCompleteSemaphores[m_FrameIndex],
            nullptr
        ).value;

        // Get command buffer and begin rendering
        VulkanCommandBuffer* cmd = m_Frames[m_FrameIndex]->GetCommandBufferAllocator().GetOrAllocate(*this);
        cmd->BeginRendering(&sc.textures[sc.acquiredImageIndex], clearColor);
        m_FrameSwapChainPresentations.push_back(renderTarget);

        return cmd;
    }

    void VulkanGraphicsDevice::EndPass(ICommandBuffer* cmd)
    {
        ENGINE_ASSERT(cmd != nullptr, "Vulkan: VulkanGraphicsDevice: EndPass(): cmd is nullptr!");

        // End command buffer
        VulkanCommandBuffer* vcmd = static_cast<VulkanCommandBuffer*>(cmd);
        vcmd->EndRendering();

        // Swapchain data
        SwapChainHandle sc = m_FrameSwapChainPresentations.back();
        VulkanSwapChainData& scData = GetSwapChainData(sc);

        // Semaphores
        vk::Semaphore presentComplete = *scData.presentCompleteSemaphores[m_FrameIndex];
        vk::Semaphore renderFinished  = *scData.renderFinishedSemaphores[scData.acquiredImageIndex];

        // Submit info
        m_FrameCommandBuffers.push_back(*vcmd->GetCommandBuffer());
        m_FrameWaitSemaphores.push_back(presentComplete);
        m_FrameSignalSemaphores.push_back(renderFinished);
        m_FrameStageFlags.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &m_FrameWaitSemaphores.back();
        submitInfo.pWaitDstStageMask    = &m_FrameStageFlags.back();
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &m_FrameCommandBuffers.back();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &m_FrameSignalSemaphores.back();

        m_FrameSubmits.push_back(submitInfo);
    }

    // Immediate command buffer
    ICommandBuffer* VulkanGraphicsDevice::BeginImmediate()
    {
        ENGINE_CORE_ASSERT(m_InImmediatePass == false, "Vulkan: VulkanGraphicsDevice: BeginImmediate(): already in immediate pass!");
        VulkanCommandBuffer* cmd = m_ImmediateCommandBuffer.get();
        cmd->Reset();
        cmd->BeginImmediate();
        m_InImmediatePass = true;
        return cmd;
    }

    void VulkanGraphicsDevice::EndImmediate(ICommandBuffer* cmd)
    {
        ENGINE_ASSERT(cmd != nullptr, "Vulkan: VulkanGraphicsDevice: EndImmediate(): cmd is nullptr!");
        ENGINE_CORE_ASSERT(cmd == m_ImmediateCommandBuffer.get(), "Vulkan: VulkanGraphicsDevice: EndImmediate(): cmd is not immediate command buffer!");
        ENGINE_CORE_ASSERT(m_InImmediatePass != false, "Vulkan: VulkanGraphicsDevice: EndImmediate(): not in immediate pass!");

        m_ImmediateCommandBuffer->EndImmediate();

        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*(m_ImmediateCommandBuffer->GetCommandBuffer());

        m_Context.GetGraphicsQueue().queue.submit(submitInfo, *m_ImmediateFence);

        m_Context.GetDevice().waitForFences(*m_ImmediateFence, VK_TRUE, UINT64_MAX);
        m_Context.GetDevice().resetFences(*m_ImmediateFence);

        m_ImmediateCommandBuffer->Reset();
        m_InImmediatePass = false;
    }

    // Swapchain configuration
    void VulkanGraphicsDevice::ResizeSwapChain(SwapChainHandle swapchain, uint32_t width, uint32_t height)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(swapchain);

        if(sc.width != width || sc.height != height)
        {
            sc.width = width;
            sc.height = height;
            sc.needsRebuild = true;
        }
    } 

    void VulkanGraphicsDevice::SetSwapChainPresentMode(SwapChainHandle swapchain, PresentMode mode)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(swapchain);

        if(sc.presentMode != mode)
        {
            sc.presentMode = mode;
            sc.needsRebuild = true;
        }
    }


    // Destroy
    void VulkanGraphicsDevice::OnDestroy()
    {
        m_Context.GetDevice().waitIdle();
    }

    // Swapchains
    void VulkanGraphicsDevice::RebuildSwapchain(VulkanSwapChainData& swapChainData)
    {
        m_Context.GetDevice().waitIdle();

        vk::SurfaceCapabilitiesKHR capabilities = m_Context.GetPhysicalDevice().getSurfaceCapabilitiesKHR(*swapChainData.surface);

        // Extent
        swapChainData.extent = VulkanCommon::GetSurfaceExtent(
            capabilities,
            swapChainData.width,
            swapChainData.height
        );

        // Skip if width or height is 0
        if (swapChainData.extent.width == 0 || swapChainData.extent.height == 0)
            return;

        // Clear
        swapChainData.renderFinishedSemaphores.clear();
        swapChainData.presentCompleteSemaphores.clear();
        swapChainData.textures.clear();
        swapChainData.images.clear();

        // Format
        swapChainData.surfaceFormat = VulkanCommon::ChooseSurfaceFormat(
            m_Context.GetPhysicalDevice().getSurfaceFormatsKHR(*swapChainData.surface),
            VulkanCommon::GetSurfaceFormat(swapChainData.format)
        );
        ENGINE_CORE_ASSERT(swapChainData.surfaceFormat.format != vk::Format::eUndefined, "Vulkan: VulkanGraphicsDevice: CreateSwapChain(): Surface format is not supported!");
        
        // Present mode
        swapChainData.vkPresentMode = VulkanCommon::ChoosePresentMode(
            m_Context.GetPhysicalDevice().getSurfacePresentModesKHR(*swapChainData.surface),
            VulkanCommon::GetPresentMode(swapChainData.presentMode)
        );
        if(swapChainData.vkPresentMode != VulkanCommon::GetPresentMode(swapChainData.presentMode))
        {
            LOG_CORE_WARN("Vulkan: VulkanGraphicsDevice: CreateSwapChain(): Present mode is unsupported. Falling back to PresentMode::Immediate");
            swapChainData.vkPresentMode = vk::PresentModeKHR::eImmediate;
            swapChainData.presentMode = PresentMode::Immediate;
        }

        // Min images
        uint32_t minImages = VulkanCommon::GetSurfaceMinImageCount(capabilities);

        // Swapchain
        vk::SwapchainCreateInfoKHR swapChainCreateInfo(
            {},
            *swapChainData.surface,
            minImages,
            swapChainData.surfaceFormat.format,
            swapChainData.surfaceFormat.colorSpace,
            swapChainData.extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            0, nullptr,
            capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            swapChainData.vkPresentMode,
            true,
            *swapChainData.swapchain
        );
        
        swapChainData.swapchain = vk::raii::SwapchainKHR(m_Context.GetDevice(), swapChainCreateInfo);
        swapChainData.images = swapChainData.swapchain.getImages();

        // Textures
        TextureDesc texDesc{
            .width = swapChainData.extent.width,
            .height = swapChainData.extent.height,
            .format = swapChainData.format,
            .usage = TextureUsage::RenderTarget
        };

        // Semaphores
        vk::SemaphoreCreateInfo semaphoreCreateInfo{};

        // Textures and RenderFinishedSemaphores: per swapchain image
        for(auto &image : swapChainData.images)
        {
            // Create texture
            VulkanTextureData& texData = swapChainData.textures.emplace_back();
            texData.desc = texDesc;
            texData.format = swapChainData.surfaceFormat.format; // format selection guaruntees that vk::SurfaceFormatKHR and PixelFormat match.
                                                                 // desc.format isn't even used here anyway.
            texData.image = image;
            texData.ownsImage = false;

            CreateImageView(texData);

            // Create new render finished semaphore
            swapChainData.renderFinishedSemaphores.emplace_back(m_Context.GetDevice(), semaphoreCreateInfo);
        }

        // PresentCompleteSemaphores: per global frame
        for(int i = 0; i < k_MaxFramesInFlight; i++)
        {
            swapChainData.presentCompleteSemaphores.emplace_back(m_Context.GetDevice(), semaphoreCreateInfo);
        }

        swapChainData.needsRebuild = false;
    }

    void VulkanGraphicsDevice::CreateImageView(VulkanTextureData& textureData)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            textureData.image,
            vk::ImageViewType::e2D,
            textureData.format,
            {},
            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
        );

        textureData.imageView = vk::raii::ImageView(m_Context.GetDevice(), imageViewCreateInfo);
    }

    void VulkanGraphicsDevice::CreateSampler(VulkanTextureData& textureData)
    {
        // Create sampler
        // TODO: Vulkan: Texture sampling options
        // Hardcoded: linear sampling, repeat
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = vk::False;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = vk::False;
        samplerInfo.compareEnable = vk::False;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

        textureData.sampler = vk::raii::Sampler(m_Context.GetDevice(), samplerInfo);
    }

    // Public getters for Vulkan classes
    VulkanBufferData& VulkanGraphicsDevice::GetBufferData(BufferHandle buffer)
    {
        ENGINE_CORE_ASSERT(buffer.IsValid(), "Vulkan: VulkanGraphicsDevice: GetBufferData: buffer is invalid!");
    
        auto it = m_Buffers.find(buffer.id);
        ENGINE_ASSERT(it != m_Buffers.end(), "Vulkan: VulkanGraphicsDevice: GetBufferData: buffer is not found!");
        
        return it->second;
    }

    VulkanTextureData& VulkanGraphicsDevice::GetTextureData(TextureHandle texture)
    {
        ENGINE_CORE_ASSERT(texture.IsValid(), "Vulkan: VulkanGraphicsDevice: GetTextureData: texture is invalid!");
    
        auto it = m_Textures.find(texture.id);
        ENGINE_ASSERT(it != m_Textures.end(), "Vulkan: VulkanGraphicsDevice: GetTextureData: texture is not found!");
        
        return it->second;
    }

    VulkanShaderData& VulkanGraphicsDevice::GetShaderData(ShaderHandle shader)
    {
        ENGINE_CORE_ASSERT(shader.IsValid(), "Vulkan: VulkanGraphicsDevice: GetShaderData: shader is invalid!");
    
        auto it = m_Shaders.find(shader.id);
        ENGINE_ASSERT(it != m_Shaders.end(), "Vulkan: VulkanGraphicsDevice: GetShaderData: shader is not found!");
        
        return it->second;
    }

    VulkanPipelineData& VulkanGraphicsDevice::GetPipelineData(PipelineHandle pipeline)
    {
        ENGINE_CORE_ASSERT(pipeline.IsValid(), "Vulkan: VulkanGraphicsDevice: GetPipelineData: pipeline is invalid!");
    
        auto it = m_Pipelines.find(pipeline.id);
        ENGINE_ASSERT(it != m_Pipelines.end(), "Vulkan: VulkanGraphicsDevice: GetPipelineData: pipeline is not found!");
        
        return it->second;
    }

    VulkanSwapChainData& VulkanGraphicsDevice::GetSwapChainData(SwapChainHandle swapchain)
    {
        ENGINE_CORE_ASSERT(swapchain.IsValid(), "Vulkan: VulkanGraphicsDevice: GetSwapChainData: swapchain is invalid!");
    
        auto it = m_SwapChains.find(swapchain.id);
        ENGINE_ASSERT(it != m_SwapChains.end(), "Vulkan: VulkanGraphicsDevice: GetSwapChainData: swapchain is not found!");
        
        return it->second;
    }
} // namespace Engine::RHI::Vulkan
