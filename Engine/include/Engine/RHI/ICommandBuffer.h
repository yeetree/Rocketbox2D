#ifndef ENGINE_RHI_ICOMMANDBUFFER
#define ENGINE_RHI_ICOMMANDBUFFER

#include "engine_export.h"

#include "Engine/Core/Base.h"

namespace Engine
{
    class ENGINE_EXPORT ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
}

#endif // ENGINE_RHI_ICOMMANDBUFFER
