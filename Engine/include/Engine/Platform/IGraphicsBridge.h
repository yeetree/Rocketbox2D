#ifndef ENGINE_PLATFORM_IGRAPHICSBRIDGE
#define ENGINE_PLATFORM_IGRAPHICSBRIDGE

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"

namespace Engine
{
    /*  
    This is used to link platforms with graphics apis
    To explain, I'm going to use the scenario of SDL3 + Vulkan

    Instead of creating SDL3VulkanPlatform or SDL3VulkanGraphicsDevice,
    I will first create a IVulkanGraphicsBridge.
    This will declare Vulkan-specific functions such as CreateSurface or GetExtensions

    Then, I will create a SDL3VulkanGraphicsBridge. This implements the functions that
    IVulkanGraphicsBridge declares. This results in zero redundancy in crossing
    platforms and graphics apis. I.E. This is only code that needs to know both platform
    and api, and IGraphicsDevice and other RHI classes don't need to worry about it.
    
    IPlatform will be passed a GraphicsAPI upon creation and then pass it to the static 
    IGraphicsBridge Create function. This will be the only place that knows all combinations
    of platforms and graphics apis

    This way, Vulkan RHI classes only need to know about the existance of IVulkanGraphicsBridge
    and not need to worry about the specific implementation. If it needs a surface, it can ask
    the graphics bridge and not need to worry about if it's using SDL or not.
    */

    class IGraphicsBridge {
    public:
        virtual ~IGraphicsBridge() = default;

        static Scope<IGraphicsBridge> Create(GraphicsAPI api);
    };
} // namespace Engine


#endif // ENGINE_PLATFORM_IGRAPHICSBRIDGE
