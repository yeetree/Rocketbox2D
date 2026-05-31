#ifndef ENGINE_RESOURCES_RESOURCEMANAGER
#define ENGINE_RESOURCES_RESOURCEMANAGER

#include "engine_export.h"

#include <any>
#include <map>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Engine/Core/Base.h"
#include "Engine/Core/Handle.h"
#include "Engine/Core/Assert.h"
#include "Engine/Resources/IResource.h"
#include "Engine/Resources/IResourceLoader.h"

namespace Engine
{
    class ENGINE_EXPORT ResourceManager
    {
    private:
        struct Slot {
            Scope<IResource> resource;
            uint32_t         version = 0;
        };

        std::unordered_map<std::type_index, Scope<IResourceLoader>>                    m_Loaders;
        std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>> m_Identifiers; // std::string -> VersionedHandle<T>

        std::unordered_map<std::type_index, std::vector<Slot>> m_Slots; // Slot index = handle.id + 1 (0 is reserved)
        std::unordered_map<std::type_index, std::vector<uint32_t>> m_FreeIDs;

        template<typename T>
        uint32_t GetOrAllocateSlot(const std::string& identifier)
        {
            auto& innerMap = m_Identifiers[typeid(T)];
            auto it = innerMap.find(identifier);

            // Existing slot
            if (it != innerMap.end()) {
                VersionedHandle<T> oldHandle = std::any_cast<VersionedHandle<T>>(it->second);
                uint32_t index = oldHandle.id - 1;

                // Clear slot data and invalidate existing handles
                Slot& slot = m_Slots[typeid(T)][index];
                // If loader exists, use it to unload. Else, just reset the pointer
                auto loaderIt = m_Loaders.find(typeid(T));
                if(loaderIt != m_Loaders.end())
                {
                    loaderIt->second->Unload(std::move(slot.resource));
                }
                slot.resource.reset();
                slot.version++;
                
                return index;
            }

            // Reuse freed slot
            auto& freeIDs = m_FreeIDs[typeid(T)];
            if (!freeIDs.empty()) {
                uint32_t index = freeIDs.back();
                freeIDs.pop_back();
                return index;
            }

            // Allocate slot
            auto& slots = m_Slots[typeid(T)];
            uint32_t index = static_cast<uint32_t>(slots.size());
            slots.emplace_back();
            return index;
        }

    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        // No copying!
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        // Load a resource by name using the loader. If resource exists,
        // it will be overwritten and its handles will be invalidated.
        template<typename T>
        VersionedHandle<T> Load(const std::string& identifier, const ResourceLoadDesc& desc)
        {
            auto loaderIt = m_Loaders.find(typeid(T));
            ENGINE_CORE_ASSERT(loaderIt != m_Loaders.end(), "ResourceManager: Load<T>(): No resource loader registered for type!");

            // Load
            Scope<IResource> resource = loaderIt->second->Load(desc);
            if (!resource) return {};

            uint32_t index = GetOrAllocateSlot<T>(identifier);

            // Update slot
            Slot& slot = m_Slots[typeid(T)][index];
            slot.resource = std::move(resource);

            // Create handle
            VersionedHandle<T> handle;
            handle.id = index + 1; // 0 is reserved
            handle.version = slot.version;

            // Map identifier to string
            m_Identifiers[typeid(T)][identifier] = handle;

            return handle;
        }

        // Create a resource by name using the registered loader. If resource exists,
        // it will be overwritten and its handles will be invalidated.
        template<typename T>
        VersionedHandle<T> Create(const std::string& identifier, const ResourceCreateDesc& desc)
        {
            auto loaderIt = m_Loaders.find(typeid(T));
            ENGINE_CORE_ASSERT(loaderIt != m_Loaders.end(), "ResourceManager: Create<T>(): No resource loader registered for type!");

            // Load
            Scope<IResource> resource = loaderIt->second->Create(desc);
            if (!resource) return {};

            // Update slot
            uint32_t idx = GetOrAllocateSlot<T>(identifier);
            Slot& slot = m_Slots[typeid(T)][idx];
            slot.resource = std::move(resource);

            // Create handle
            VersionedHandle<T> handle;
            handle.id = idx + 1; // 0 is reserved
            handle.version = slot.version;

            // Map identifier to string
            m_Identifiers[typeid(T)][identifier] = handle;

            return handle;
        }

        // Set a resource by name
        template<typename T>
        VersionedHandle<T> Set(const std::string& identifier, Scope<T> resource)
        {
            if (!resource) return {};

            // Update slot
            uint32_t index = GetOrAllocateSlot<T>(identifier);
            Slot& slot = m_Slots[typeid(T)][index];
            slot.resource = std::move(resource); 

            // Create handle
            VersionedHandle<T> handle;
            handle.id = index + 1; // 0 is reserved
            handle.version = slot.version;

            // Map identifier to string
            m_Identifiers[typeid(T)][identifier] = handle;

            return handle;
        }

        // Unloads a resource
        template<typename T>
        void Unload(VersionedHandle<T> handle)
        {
            if (!IsValid(handle)) return;

            // Get slot
            uint32_t index = handle.id - 1;
            Slot& slot = m_Slots[typeid(T)][index];

            // Clear slot resource and invalidate existing handles
            // If loader exists, use it. If not, then just reset the pointer
            auto loaderIt = m_Loaders.find(typeid(T));
            if(loaderIt != m_Loaders.end())
            {
                loaderIt->second->Unload(std::move(slot.resource));
            }
            slot.resource.reset();

            slot.version++;

            // Unmap identifer map
            for (auto it = m_Identifiers[typeid(T)].begin(); it != m_Identifiers[typeid(T)].end(); ++it) {
                auto storedHandle = std::any_cast<VersionedHandle<T>>(it->second);
                if (storedHandle.id == handle.id) {
                    m_Identifiers[typeid(T)].erase(it);
                    break;
                }
            }

            // Mark ID as free
            m_FreeIDs[typeid(T)].push_back(index);
        }

        // Get existing resource handle by name
        template<typename T>
        VersionedHandle<T> Find(const std::string& identifier) const
        {
            // Find resource std::string -> handle map
            auto mapIt = m_Identifiers.find(typeid(T));
            if(mapIt == m_Identifiers.end()) return {}; // Not found

            // Find handle
            auto handleIt = mapIt->second.find(identifier);
            if(handleIt == mapIt->second.end()) return {}; // Not found

            // Cast to VersionedHandle<T>
            return std::any_cast<VersionedHandle<T>>(handleIt->second);
        }

        // Check if an identifier points to an existing resource
        template<typename T>
        bool Has(const std::string& identifier) const
        {
            return IsValid(Find<T>(identifier));
        }
        
        template<typename T>
        bool IsValid(VersionedHandle<T> handle) const
        {
            if (handle.id == 0 && handle.version == 0) return false; 

            auto poolIt = m_Slots.find(typeid(T));
            if (poolIt == m_Slots.end()) return false;

            uint32_t index = handle.id - 1;
            if (index >= poolIt->second.size()) return false;

            const Slot& slot = poolIt->second[index];
            return (slot.version == handle.version && slot.resource != nullptr);
        }

        template<typename T>
        T* Get(VersionedHandle<T> handle) const
        {
            if (!IsValid(handle)) return nullptr;
    
            auto poolIt = m_Slots.find(typeid(T));
            if (poolIt == m_Slots.end()) return nullptr;

            uint32_t index = handle.id - 1;
            return static_cast<T*>(poolIt->second[index].resource.get());
        }

        template<typename T>
        void RegisterLoader(Scope<IResourceLoader> loader)
        {
            m_Loaders[typeid(T)] = std::move(loader);
        }
    };

} // namespace Engine

#endif // ENGINE_RESOURCES_RESOURCEMANAGER
