#ifndef ENGINE_RESOURCES_IRESOURCELOADER
#define ENGINE_RESOURCES_IRESOURCELOADER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Resources/IResource.h"

namespace Engine
{
    class ENGINE_EXPORT IResourceLoader
    {
    public:
        virtual ~IResourceLoader() = default;
        
        virtual Scope<IResource> Load(const ResourceLoadDesc& info) = 0;
        virtual Scope<IResource> Create(const ResourceCreateDesc& info) = 0;
        virtual void Unload(Scope<IResource> resource) = 0;
    };
} // namespace Engine


#endif // ENGINE_RESOURCES_IRESOURCELOADER
