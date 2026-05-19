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
        std::unordered_map<uint32_t, std::string> m_HashToString;

    public:
        StringRegistry() = default;
        ~StringRegistry();

        size_t GetID(const std::string& str);
        std::string_view GetString(uint32_t id) const;

        void Clear();
    };
} // namespace Engine


#endif // ENGINE_CORE_STRINGID