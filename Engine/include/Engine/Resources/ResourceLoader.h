#ifndef ENGINE_RESOURCES_RESOURCELOADER
#define ENGINE_RESOURCES_RESOURCELOADER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Resources/Resource.h"

namespace Engine
{
    class ENGINE_EXPORT IResourceLoader
    {
    public:
        virtual ~IResourceLoader() = default;
        
        virtual Scope<Resource> Load(const ResourceInfo& info) = 0;
        virtual Scope<Resource> Create(const ResourceInfo& info) = 0;
    };
} // namespace Engine


#endif // ENGINE_RESOURCES_RESOURCELOADER
