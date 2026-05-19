#include "Engine/Core/StringRegistry.h"
#include "Engine/Core/Hash.h"

namespace Engine
{

    StringRegistry::~StringRegistry()
    {
        Clear();
    }

    size_t StringRegistry::GetID(const std::string& str)
    {
        if(str.length() == 0)
        {
            return 0;
        }

        size_t hash = Hash32(str);
        hash = (hash == 0) ? 1 : hash; // Reserve 0

        auto it = m_HashToString.find(hash);
        if(it == m_HashToString.end())
        {
            m_HashToString.emplace(hash, str);
        }
        return hash;
    }

    std::string_view StringRegistry::GetString(uint32_t id) const
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

    void StringRegistry::Clear()
    {
        m_HashToString.clear();
    }
} // namespace Engine
