#ifndef ENGINE_CORE_SERVICELOCATOR
#define ENGINE_CORE_SERVICELOCATOR

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace Engine
{
    // from https://github.com/Seng3694/ServiceLocator/
    class ENGINE_EXPORT ServiceLocator
    {
    public:
        ServiceLocator();
        ~ServiceLocator();

        void Clear();

        template<typename T>
	    void RegisterInstance(T* instance = new T());

        template<typename T>
	    void RegisterCreator(std::function<Ref<T>()> creator = []() { return CreateRef<T>(); });

        template<typename T>
	    Ref<T> Get() const;

    private:
        std::unordered_map<size_t, Ref<void>> m_Instances;
        std::unordered_map<size_t, std::function<Ref<void>()>> m_Creators;
    };
} // namespace Engine

#include "ServiceLocator.inl"

#endif // ENGINE_CORE_SERVICELOCATOR
