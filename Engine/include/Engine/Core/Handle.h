#ifndef ENGINE_CORE_HANDLE
#define ENGINE_CORE_HANDLE

#include <cstdint>
#include <functional>
#include <cstddef>

namespace Engine
{
    template<typename Tag>
    struct Handle
    {
        uint32_t id = 0;
        static uint32_t AllocateID()
        {
            static uint32_t nextID = 1;
            return nextID++;
        }
        bool IsValid() const { return id != 0; }
        bool operator==(const Handle<Tag>& other) const {
            return this->id == other.id;
        }
    };

    template<typename Tag>
    struct VersionedHandle
    {
        uint32_t id = 0;
        uint32_t version = 0;
        // Must use external manager to check validity
        bool operator==(const VersionedHandle<Tag>& other) const {
            return this->id == other.id && this->version == other.version;
        }
    };

} // namespace Engine

// For use in unordered_map
namespace std {
    template<typename Tag>
    struct hash<Engine::Handle<Tag>> {
        std::size_t operator()(const Engine::Handle<Tag>& h) const noexcept {
            return std::hash<uint32_t>{}(h.id);
        }
    };

    template<typename Tag>
    struct hash<Engine::VersionedHandle<Tag>> {
        std::size_t operator()(const Engine::VersionedHandle<Tag>& h) const noexcept {
            size_t seed = std::hash<uint32_t>{}(h.id);
            seed ^= std::hash<uint32_t>{}(h.version) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}


#endif // ENGINE_CORE_HANDLE
