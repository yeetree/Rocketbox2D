#ifndef ENGINE_CORE_STRINGID
#define ENGINE_CORE_STRINGID

#include "engine_export.h"

#include <unordered_map>
#include <string>
#include <cstdint>

namespace Engine
{
    class ENGINE_EXPORT StringRegistry
    {
    private:
        StringRegistry() = default;

        static std::unordered_map<uint32_t, std::string> m_HashToString;

    public:
        static uint32_t GetID(const std::string& str);
        static std::string_view GetString(uint32_t id);

        // Clear map and register engine strings. See source/Core/EngineStrings.h
        static void Reset();
    };
} // namespace Engine


#endif // ENGINE_CORE_STRINGID