#ifndef ENGINE_CORE_HASH
#define ENGINE_CORE_HASH

#include "engine_export.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace Engine
{
    inline constexpr uint32_t Hash32(std::string_view str) {
        std::uint32_t hash = 2166136261u;
        for (char c : str) {
            hash ^= static_cast<std::uint32_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }

    inline constexpr uint64_t Hash64(std::string_view str) {
        std::uint64_t hash = 14695981039346656037ull;
        for (char c : str) {
            hash ^= static_cast<std::uint64_t>(c);
            hash *= 1099511628211ull;
        }
        return hash;
    }
} // namespace Engine

#endif // ENGINE_CORE_HASH