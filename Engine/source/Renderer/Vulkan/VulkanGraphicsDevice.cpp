#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/FileSystem.h"
#include "Renderer/Vulkan/VulkanPipelineState.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include <SDL3/SDL_vulkan.h>

namespace Engine {
    
    VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanGraphicsDevice::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        switch(severity) {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                LOG_CORE_WARN("Vulkan: Warning: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                LOG_CORE_ERROR("Vulkan: Error: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                LOG_CORE_TRACE("Vulkan: Verbose: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
        }
        
        return vk::False;
    }

    VulkanGraphicsDevice::VulkanGraphicsDevice(SDL_Window* window) : m_Window(window) {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");
        
        CreateInstance();
        SetupDebugMessanger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateGraphicsPipeline();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice() {
        if (*m_Device) {
            m_Device.waitIdle();
        }
    };

    // Setup functions
    void VulkanGraphicsDevice::CreateInstance() {
        LOG_CORE_INFO("Vulkan: Creating Vulkan instance...");

        // App info
        constexpr vk::ApplicationInfo appInfo(
            "VKTest",
            VK_MAKE_VERSION( 1, 0, 0 ),
            "Engine",
            VK_MAKE_VERSION( 1, 0, 0 ),
            vk::ApiVersion14
        );

        // Get the required layers
        std::vector<char const*> requiredLayers;
        if (enableValidationLayers) {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        // Check if the required layers are supported by the Vulkan implementation.
        auto layerProperties = m_Context.enumerateInstanceLayerProperties();
        if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
            return std::ranges::none_of(layerProperties,
                                    [requiredLayer](auto const& layerProperty)
                                    { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
        }))
        {
            LOG_CORE_ERROR("Vulkan: One or more required layers is not supported!");
            return; // Failure
        }

        // Get required extensions for SDL
        uint32_t SDLExtensionCount = 0;
        char const * const * SDLExtensions = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount);
        std::vector extensions(SDLExtensions, SDLExtensions + SDLExtensionCount);
        
        // Add Engine extensions
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName );
        }

        std::vector<vk::ExtensionProperties> extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (uint32_t i = 0; i < extensions.size(); ++i)
        {
            if (std::ranges::none_of(extensionProperties,
                                    [extension = extensions[i]](auto const& extensionProperty)
                                    { return strcmp(extensionProperty.extensionName, extension) == 0; }))
            {
                LOG_CORE_ERROR("Vulkan: Required extension not supported: {0}", std::string(extensions[i]));
                return; // Failure
            }
        }

        // Create instance
        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data(); 

        m_Instance = vk::raii::Instance(m_Context, createInfo);
    }

    void VulkanGraphicsDevice::SetupDebugMessanger() {
        // Set up debug messanger
        if(!enableValidationLayers) { return; }

        LOG_CORE_INFO("Vulkan: Setting up Debug Messanger...");

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );
        vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
        debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
        debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
        debugUtilsMessengerCreateInfoEXT.pfnUserCallback = &DebugCallback;
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    void VulkanGraphicsDevice::CreateSurface() {
        LOG_CORE_INFO("Vulkan: Creating surface...");

        // Thank god we use SDL
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(m_Window, *m_Instance, NULL, &surface)) {
            LOG_CORE_ERROR("Vulkan: Could not create surface: SDL: {0}", SDL_GetError());
            return; // Failure
        }
        m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
    }

    void VulkanGraphicsDevice::PickPhysicalDevice() {
        LOG_CORE_INFO("Vulkan: Selecting Physical Device...");
        
        // Select physical device
        std::vector<vk::raii::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            LOG_CORE_ERROR("Vulkan: Failed to find GPUs with Vulkan support!");
            return; // Failure
        }

        // Loop through devices
        const auto devIter = std::ranges::find_if(devices,
        [&](auto const & device) {
                auto queueFamilies = device.getQueueFamilyProperties();
                bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
                const auto qfpIter = std::ranges::find_if(queueFamilies,
                []( vk::QueueFamilyProperties const & qfp )
                {
                    return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
                } );
                isSuitable = isSuitable && ( qfpIter != queueFamilies.end() );
                auto extensions = device.enumerateDeviceExtensionProperties( );
                bool found = true;
                for (auto const & extension : deviceExtensions) {
                    auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                    found = found &&  extensionIter != extensions.end();
                }
                isSuitable = isSuitable && found;
                if (isSuitable) {
                    m_PhysicalDevice = device;
                }
                return isSuitable;
        });
        if (devIter == devices.end()) {
            LOG_CORE_ERROR("Vulkan: Failed to find suitable GPU!");
            return; // Failure
        }
    }

    void VulkanGraphicsDevice::CreateLogicalDevice() {
        LOG_CORE_INFO("Vulkan: Creating Logical Device...");

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports both graphics and present
		for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			    m_PhysicalDevice.getSurfaceSupportKHR(qfpIndex, *m_Surface))
			{
				// found a queue family that supports both graphics and present
				m_QueueIndex= qfpIndex;
				break;
			}
		}
		if (m_QueueIndex == ~0)
		{
            LOG_CORE_ERROR("Vulkan: Could not find a queue for graphics and present!");
			return; // Failure
		}

        // query for Vulkan 1.3 features
        vk::PhysicalDeviceVulkan11Features vulkan11Features;
        vulkan11Features.shaderDrawParameters = true;
        
        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vulkan13Features.dynamicRendering = true;
        vulkan13Features.synchronization2 = true;
        
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
        extendedDynamicStateFeatures.extendedDynamicState = true;
    
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain(
            vk::PhysicalDeviceFeatures2{},
            vulkan11Features,
            vulkan13Features,
            extendedDynamicStateFeatures
        );

        // create a Device
        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
        deviceQueueCreateInfo.queueFamilyIndex = m_QueueIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);
        m_Queue = vk::raii::Queue(m_Device, m_QueueIndex, 0);
    }

    void VulkanGraphicsDevice::CreateSwapChain() {
        LOG_CORE_INFO("Vulkan: Creating Swap Chain...");

        // Get surface formats and extent
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR( m_Surface );
        m_SwapChainSurfaceFormat = ChooseSwapSurfaceFormat(m_PhysicalDevice.getSurfaceFormatsKHR( m_Surface ));
        m_SwapChainExtent = ChooseSwapExtent(surfaceCapabilities);

        // Create swapchain
        vk::SwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);
        swapChainCreateInfo.imageFormat = m_SwapChainSurfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = m_SwapChainSurfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = m_SwapChainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapChainCreateInfo.presentMode = ChooseSwapPresentMode(m_PhysicalDevice.getSurfacePresentModesKHR( m_Surface ));
        swapChainCreateInfo.clipped = true;
        swapChainCreateInfo.oldSwapchain = nullptr;

        m_SwapChain = vk::raii::SwapchainKHR( m_Device, swapChainCreateInfo );
        m_SwapChainImages = m_SwapChain.getImages();
    }

    void VulkanGraphicsDevice::CreateImageViews() {
        LOG_CORE_INFO("Vulkan: Creating Image Views...");
        m_SwapChainImageViews.clear();

        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = m_SwapChainSurfaceFormat.format;
        imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        for (auto& image : m_SwapChainImages)
		{
			imageViewCreateInfo.image = image;
			m_SwapChainImageViews.emplace_back(m_Device, imageViewCreateInfo);
		}
    }

    void VulkanGraphicsDevice::CreateGraphicsPipeline() {
        LOG_CORE_INFO("Vulkan: Creating Graphics Pipeline...");
        
        std::vector<char> shaderCode = Engine::FileSystem::ReadFile(FileSystem::GetAbsolutePath("./shaders/slang.spv"));
        ShaderDesc shaderDesc;
        shaderDesc.stages[ShaderStage::Vertex].byteCode = shaderCode;
        shaderDesc.stages[ShaderStage::Vertex].entryPoint = "vertMain";
        shaderDesc.stages[ShaderStage::Fragment].byteCode = shaderCode;
        shaderDesc.stages[ShaderStage::Fragment].entryPoint = "fragMain";
        
        m_Shader = CreateShader(shaderDesc);

        PipelineDesc pipeDesc;
        pipeDesc.shader = m_Shader.get();

        m_Pipeline = CreatePipelineState(pipeDesc);
    }

    void VulkanGraphicsDevice::CreateCommandPool() {
        LOG_CORE_INFO("Vulkan: Creating Command Pool...");
		vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = m_QueueIndex;
		m_CommandPool = vk::raii::CommandPool(m_Device, poolInfo);
	}

	void VulkanGraphicsDevice::CreateCommandBuffer() {
        LOG_CORE_INFO("Vulkan: Creating Command Buffers...");
        m_CommandBuffers.clear();
        vk::CommandBufferAllocateInfo allocInfo(m_CommandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
        m_CommandBuffers = vk::raii::CommandBuffers( m_Device, allocInfo );
    }

    void VulkanGraphicsDevice::CreateSyncObjects() {
        LOG_CORE_INFO("Vulkan: Creating Sync Objects...");
        m_RenderFinishedSemaphores.clear();
        m_PresentCompleteSemaphores.clear();
        m_InFlightFences.clear();

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			m_RenderFinishedSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_PresentCompleteSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());
			m_InFlightFences.emplace_back(m_Device, vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
		}
    }

    // Utility

    [[nodiscard]] vk::raii::ShaderModule VulkanGraphicsDevice::CreateShaderModule(const std::vector<char>& code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(char);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        vk::raii::ShaderModule shaderModule{ m_Device, createInfo };
        return shaderModule;
    }

    vk::SurfaceFormatKHR VulkanGraphicsDevice::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanGraphicsDevice::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }
        return vk::PresentModeKHR::eFifo; // Garunteed
    }

    vk::Extent2D VulkanGraphicsDevice::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        int width, height;
        SDL_GetWindowSizeInPixels(m_Window, &width, &height);

        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    uint32_t VulkanGraphicsDevice::ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
		if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
		{
			minImageCount = surfaceCapabilities.maxImageCount;
		}
		return minImageCount;
    }

    void VulkanGraphicsDevice::TransitionImageLayout(
	    uint32_t                imageIndex,
	    vk::ImageLayout         old_layout,
	    vk::ImageLayout         new_layout,
	    vk::AccessFlags2        src_access_mask,
	    vk::AccessFlags2        dst_access_mask,
	    vk::PipelineStageFlags2 src_stage_mask,
	    vk::PipelineStageFlags2 dst_stage_mask)
	{
		vk::ImageMemoryBarrier2 barrier;
        barrier.srcStageMask        = src_stage_mask;
	    barrier.srcAccessMask       = src_access_mask;
		barrier.dstStageMask        = dst_stage_mask;
		barrier.dstAccessMask       = dst_access_mask;
		barrier.oldLayout           = old_layout;
		barrier.newLayout           = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image               = m_SwapChainImages[imageIndex];
		barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel   = 0;
		barrier.subresourceRange.levelCount     = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = 1;
		vk::DependencyInfo dependency_info;
		dependency_info.dependencyFlags         = {};
		dependency_info.imageMemoryBarrierCount = 1;
		dependency_info.pImageMemoryBarriers    = &barrier;
		m_CommandBuffers[m_FrameIndex].pipelineBarrier2(dependency_info);
	}

    // Resource Creation
    Scope<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return nullptr;
    }

    Scope<ITexture> VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return nullptr;
    }

    Scope<IShader> VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return CreateScope<VulkanShader>(m_Device, desc);
    }

    Scope<IVertexArray> VulkanGraphicsDevice::CreateVertexArray(const VertexArrayDesc& desc) {
        return nullptr;
    }

    Scope<IPipelineState> VulkanGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return CreateScope<VulkanPipelineState>(m_Device, m_SwapChainSurfaceFormat.format, desc);
    }

    // Frame Management
    void VulkanGraphicsDevice::BeginFrame() {
        // Wait for current draw fence
        auto fenceResult = m_Device.waitForFences(*m_InFlightFences[m_FrameIndex], vk::True, UINT64_MAX);
        if (fenceResult != vk::Result::eSuccess) {
            LOG_CORE_ERROR("Vulkan: Failed to wait for fence!");
        }
        
        auto [result, imageIndex] = m_SwapChain.acquireNextImage(UINT64_MAX, *m_PresentCompleteSemaphores[m_FrameIndex], nullptr);
        m_ImageIndex = imageIndex;

        m_Device.resetFences(*m_InFlightFences[m_FrameIndex]);

        m_CommandBuffers[m_FrameIndex].reset();

        m_CommandBuffers[m_FrameIndex].begin({});

        // Transition the image layout for rendering
        TransitionImageLayout(
            m_ImageIndex,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        // Set up the color attachment
        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo;
        attachmentInfo.imageView = m_SwapChainImageViews[m_ImageIndex];
        attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentInfo.clearValue = clearColor;
    

        // Set up the rendering info
        vk::RenderingInfo renderingInfo;
        renderingInfo.renderArea = { .offset = { 0, 0 }, .extent = m_SwapChainExtent };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &attachmentInfo;

        // Begin rendering
        m_CommandBuffers[m_FrameIndex].beginRendering(renderingInfo);
        VulkanPipelineState* graphicsPipeline = static_cast<VulkanPipelineState*>(m_Pipeline.get());
        m_CommandBuffers[m_FrameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline->m_Pipeline);
		m_CommandBuffers[m_FrameIndex].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width), static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f));
		m_CommandBuffers[m_FrameIndex].setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_SwapChainExtent));
		m_CommandBuffers[m_FrameIndex].draw(3, 1, 0, 0);
		m_CommandBuffers[m_FrameIndex].endRendering();
		// After rendering, transition the swapchain image to PRESENT_SRC
		TransitionImageLayout(
		    m_ImageIndex,
		    vk::ImageLayout::eColorAttachmentOptimal,
		    vk::ImageLayout::ePresentSrcKHR,
		    vk::AccessFlagBits2::eColorAttachmentWrite,                // srcAccessMask
		    {},                                                        // dstAccessMask
		    vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		    vk::PipelineStageFlagBits2::eBottomOfPipe                  // dstStage
		);
		m_CommandBuffers[m_FrameIndex].end();
    }

    void VulkanGraphicsDevice::EndFrame() {
        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*m_PresentCompleteSemaphores[m_FrameIndex]; 
        submitInfo.pWaitDstStageMask = &waitDestinationStageMask;
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*m_CommandBuffers[m_FrameIndex];
        
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*m_RenderFinishedSemaphores[m_ImageIndex]; 

        m_Queue.submit(submitInfo, *m_InFlightFences[m_FrameIndex]);
    }

    void VulkanGraphicsDevice::Present() {
       vk::PresentInfoKHR presentInfoKHR;
        presentInfoKHR.waitSemaphoreCount = 1;
        // Wait for rendering to finish
        presentInfoKHR.pWaitSemaphores = &*m_RenderFinishedSemaphores[m_ImageIndex]; 
        presentInfoKHR.swapchainCount = 1;
        presentInfoKHR.pSwapchains = &*m_SwapChain;
        presentInfoKHR.pImageIndices = &m_ImageIndex;

        auto result = m_Queue.presentKHR(presentInfoKHR);
        
        switch (result)
		{
			case vk::Result::eSuccess:
				break;
			case vk::Result::eSuboptimalKHR:
				LOG_CORE_WARN("Vulkan: vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !");
				break;
			default:
				break;        // an unexpected result is returned!
		}

        m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanGraphicsDevice::SetClearColor(Vec4 color) {

    }

    // Draw call
    void VulkanGraphicsDevice::SubmitDraw(uint32_t indexCount) {
        
    }

    // Resize
    void VulkanGraphicsDevice::Resize(int width, int height) {
        if (width == 0 || height == 0) return;

        m_Device.waitIdle();

        //Get capabilities and recreate swapchain
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
        m_SwapChainExtent = ChooseSwapExtent(surfaceCapabilities);

        vk::SwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);
        swapChainCreateInfo.imageFormat = m_SwapChainSurfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = m_SwapChainSurfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = m_SwapChainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapChainCreateInfo.presentMode = ChooseSwapPresentMode(m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface));
        swapChainCreateInfo.clipped = true;
        swapChainCreateInfo.oldSwapchain = *m_SwapChain;

        // Set new swapchain (raii destroys old one)
        m_SwapChain = vk::raii::SwapchainKHR(m_Device, swapChainCreateInfo);

        // Update the image list and views
        m_SwapChainImages = m_SwapChain.getImages();
        CreateImageViews();

        // Recreate sync objects because the image count may have changed
        CreateSyncObjects();

        // Reset frame indices to start fresh
        m_FrameIndex = 0;

        LOG_CORE_INFO("Vulkan: Swapchain resized to {0} x {1} with {2} images", 
            m_SwapChainExtent.width, m_SwapChainExtent.height, m_SwapChainImages.size());
    }

} // namespace Engine
