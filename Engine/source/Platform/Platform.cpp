#include "Engine/Platform/IPlatform.h"
#include "Platform/SDL3/SDL3Platform.h"

namespace Engine
{
    void IPlatform::RegisterWindow(Ref<IWindow> window)
    {
        m_ActiveWindow = window.get();
    }

    const std::string& IPlatform::GetBasePath() const
    {
        return m_BasePath;
    }

    Scope<IPlatform> IPlatform::Create() {
        #if defined(ENGINE_PLATFORM_WINDOWS) || defined(ENGINE_PLATFORM_LINUX)
            return CreateScope<SDL3Platform>();
        #endif
    }
} // namespace Engine