#include "Engine/Core/StringRegistry.h"
#include "Engine/Core/Hash.h"
#include "Core/EngineStrings.h"


namespace Engine
{
    std::unordered_map<uint32_t, std::string> StringRegistry::m_HashToString;

    uint32_t StringRegistry::GetID(const std::string& str)
    {
        if(str.length() == 0)
        {
            return 0;
        }

        uint32_t hash = Hash32(str);
        hash = (hash == 0) ? 1 : hash; // Reserve 0

        auto it = m_HashToString.find(hash);
        if(it == m_HashToString.end())
        {
            m_HashToString.emplace(hash, str);
        }
        return hash;
    }

    std::string_view StringRegistry::GetString(uint32_t id)
    {
        if(id == 0)
        {
            return "";
        }

        auto it = m_HashToString.find(id);
        if (it != m_HashToString.end())
        {
            return it->second;
        }
        return "";
    }

    void StringRegistry::Reset()
    {
        m_HashToString.clear();
        for(const char* str : EngineStrings)
        {
            GetID(str);
        }
    }
} // namespace Engine
