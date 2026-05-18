#ifndef ENGINE_RESOURCES_IRESOURCELOADER
#define ENGINE_RESOURCES_IRESOURCELOADER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Resources/IResource.h"

namespace Engine
{
    struct ResourceInfo;

    class ENGINE_EXPORT IResourceLoader
    {
    public:
        virtual ~IResourceLoader() = default;
        
        virtual Scope<IResource> Load(const ResourceInfo& info) = 0;
        virtual Scope<IResource> Create(const ResourceInfo& info) = 0;
    };
} // namespace Engine


#endif // ENGINE_RESOURCES_IRESOURCELOADER
