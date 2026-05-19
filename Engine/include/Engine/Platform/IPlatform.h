#ifndef ENGINE_PLATFORM_IPLATFORM
#define ENGINE_PLATFORM_IPLATFORM

#include "engine_export.h"

#include "Engine/Platform/IWindow.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/GraphicsAPI.h"

#include <string>

namespace Engine
{
    class ENGINE_EXPORT IPlatform
    {
    protected:
        IWindow* m_ActiveWindow;
        std::string m_BasePath;

    public:
        virtual ~IPlatform() = default;

        virtual void PollEvents() = 0;

        virtual Scope<IWindow> CreateWindow(const WindowProperties& props) = 0;

        void RegisterWindow(Ref<IWindow> window);
        
        const std::string& GetBasePath() const;

        static Scope<IPlatform> Create();
    };
} // namespace Engine


#endif // ENGINE_PLATFORM_IPLATFORM
