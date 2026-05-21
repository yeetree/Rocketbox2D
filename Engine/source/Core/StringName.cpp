#include "Engine/Core/StringName.h"
#include "Core/EngineStrings.h"

namespace Engine
{
    std::deque<std::string> StringName::s_Strings; 
    std::unordered_map<uint32_t, size_t> StringName::s_Registry; 

    uint32_t StringName::GetID(std::string_view str)
    {
        uint32_t hash = Hash32(str); 

        auto it = s_Registry.find(hash);
        if (it != s_Registry.end())
        {
            return hash;
        }

        s_Strings.push_back(std::string(str));
        size_t allocatedIndex = s_Strings.size() - 1;

        s_Registry[hash] = allocatedIndex;

        return hash;
    }

    std::string_view StringName::GetString() const
    {
        auto it = s_Registry.find(m_Hash);
        if (it != s_Registry.end())
        {
            return s_Strings[it->second];
        }

        return "";
    }

    void StringName::Reset()
    {
        s_Strings.clear();
        s_Registry.clear();
        for(const char* str : EngineStrings)
        {
            GetID(str);
        }
    }

} // namespace Engine
