#ifndef ENGINE_RESOURCES_RESOURCE
#define ENGINE_RESOURCES_RESOURCE

#include "engine_export.h"

#include "Engine/Core/Base.h"

namespace Engine
{
    struct ResourceInfo {
        virtual ~ResourceInfo() = default;
    };

    class ENGINE_EXPORT Resource
    {
    public:
        virtual ~Resource() = default;
    };
} // namespace Engine


#endif // ENGINE_RESOURCES_RESOURCE
