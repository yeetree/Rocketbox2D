#ifndef ENGINE_RHI_ICOMMANDBUFFER
#define ENGINE_RHI_ICOMMANDBUFFER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/ITexture.h"

#include "Engine/Math/Vector.h"

namespace Engine
{
    class ENGINE_EXPORT ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        // Render target
        virtual void SetRenderTarget(Ref<ITexture> texture);

        // Textures
        virtual void Clear(Ref<ITexture> texture, Vec4 color);
    };
}

#endif // ENGINE_RHI_ICOMMANDBUFFER
