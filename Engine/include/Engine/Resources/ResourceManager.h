#ifndef ENGINE_RESOURCES_RESOURCEMANAGER
#define ENGINE_RESOURCES_RESOURCEMANAGER

#include "engine_export.h"

#include <functional>
#include <unordered_map>
#include <typeindex>

#include "Engine/Core/Base.h"

#include "Engine/Resources/IResource.h"
#include "Engine/Resources/IResourceLoader.h"

namespace Engine
{
    class IResourceLoader;

    class ENGINE_EXPORT ResourceManager {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        template<typename T>
        Ref<T> Get(const std::string& identifier) const
        {
            std::type_index typeId = typeid(T);
            auto typeIt = m_Resources.find(typeId);
            if(typeIt != m_Resources.end())
            {
                auto resIt = typeIt->second.find(identifier);
                if(resIt != typeIt->second.end())
                {
                    return std::static_pointer_cast<T>(resIt->second);
                }
            }
            return nullptr;
        }

        template<typename T>
        bool Has(const std::string& identifier) const
        {
            std::type_index typeId = typeid(T);
            auto typeIt = m_Resources.find(typeId);
            if(typeIt != m_Resources.end())
            {
                auto resIt = typeIt->second.find(identifier);
                if(resIt != typeIt->second.end())
                {
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        Ref<T> Set(const std::string& identifier, Scope<IResource> resource)
        {
            std::type_index typeId = typeid(T);
            m_Resources[typeId][identifier] = std::move(resource);
        }

        template<typename T>
        Ref<T> Load(const std::string& identifier, const ResourceInfo& info)
        {
            std::type_index typeId = typeid(T);
            auto typeIt = m_Loaders.find(typeId);
            if(typeIt != m_Loaders.end())
            {
                Scope<IResource> scope = typeIt->second->Load(info);
                if(scope)
                {
                    Ref<IResource> ref = std::move(scope);
                    m_Resources[typeId][identifier] = ref;
                    return std::static_pointer_cast<T>(ref);
                }
            }
            return nullptr;
        }

        template<typename T>
        Ref<T> Create(const std::string& identifier, const ResourceInfo& info)
        {
            std::type_index typeId = typeid(T);
            auto typeIt = m_Loaders.find(typeId);
            if(typeIt != m_Loaders.end())
            {
                Scope<IResource> scope = typeIt->second->Create(info);
                if(scope)
                {
                    Ref<IResource> ref = std::move(scope);
                    m_Resources[typeId][identifier] = ref;
                    return std::static_pointer_cast<T>(ref);
                }
            }
            return nullptr;
        }

        template<typename T>
        void RegisterLoader(Scope<IResourceLoader> loader)
        {
            std::type_index typeId = typeid(T);
            m_Loaders[typeId] = std::move(loader);
        }
        
    private:
        std::unordered_map<std::type_index, std::unordered_map<std::string, Ref<IResource>>> m_Resources;
        std::unordered_map<std::type_index, Scope<IResourceLoader>> m_Loaders;
    };
} // namespace Engine

#endif // ENGINE_RESOURCES_RESOURCEMANAGER
