#ifndef ENGINE_CORE_BASE
#define ENGINE_CORE_BASE

#include <memory>

#include "Engine/Core/PlatformDetection.h"

// From https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h

#ifdef ENGINE_DEBUG // See Engine/CMakeLists.txt
    #if defined(ENGINE_PLATFORM_WINDOWS)
        #define ENGINE_DEBUGBREAK() __debugbreak()
    #elif defined(ENGINE_PLATFORM_LINUX)
        #include <signal.h>
        #define ENGINE_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform ENGINE_DEBUGBREAK() macro not defined!"
    #endif
    #define ENGINE_ENABLE_ASSERTS
#else
    #define ENGINE_DEBUGBREAK()
#endif

#define ENGINE_EXPAND_MACRO(x) x
#define ENGINE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

namespace Engine {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    inline void HashCombine(size_t& seed, uint64_t value) {
        seed ^= std::hash<uint64_t>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}

#endif // ENGINE_CORE_BASE
