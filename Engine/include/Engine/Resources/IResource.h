#ifndef ENGINE_RESOURCES_IRESOURCE
#define ENGINE_RESOURCES_IRESOURCE

#include "engine_export.h"

#include "Engine/Core/Base.h"

namespace Engine
{
    struct ENGINE_EXPORT ResourceLoadDesc {
        virtual ~ResourceLoadDesc() = default;
    };

    struct ENGINE_EXPORT ResourceCreateDesc {
        virtual ~ResourceCreateDesc() = default;
    };

    class ENGINE_EXPORT IResource
    {
    public:
        virtual ~IResource() = default;
    };
} // namespace Engine


#endif // ENGINE_RESOURCES_IRESOURCE
