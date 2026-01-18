#ifndef ENGINE_CORE_BASE
#define ENGINE_CORE_BASE

#include <memory>

// From https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Base.h
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
}

#endif // ENGINE_CORE_BASE
