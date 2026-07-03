#ifndef ENGINE_CORE_SERVICELOCATOR
#define ENGINE_CORE_SERVICELOCATOR

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Core/Assert.h"

#include <typeindex>
#include <typeinfo>
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
        void Register(T* service)
        {
            auto typeIdx = std::type_index(typeid(T));
            m_Services[typeIdx] = static_cast<void*>(service);
        }

        template<typename T>
        void Unregister()
        {
            auto typeIdx = std::type_index(typeid(T));
            m_Services.erase(typeIdx);
        }

        template<typename T>
        T* Get() const
        {
            auto typeIdx = std::type_index(typeid(T));
            auto it = m_Services.find(typeIdx);
            
            ENGINE_CORE_ASSERT(it != m_Services.end(), "Service requested is not registered!");
            
            if (it == m_Services.end())
                return nullptr;

            return static_cast<T*>(it->second);
        }

    private:
        std::unordered_map<std::type_index, void*> m_Services;
    };
} // namespace Engine

#endif // ENGINE_CORE_SERVICELOCATOR
