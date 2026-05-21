#ifndef ENGINE_CORE_STRINGNAME
#define ENGINE_CORE_STRINGNAME

#include "engine_export.h"

#include "Engine/Core/Hash.h"

#include <deque>
#include <unordered_map>
#include <string>

namespace Engine
{
    class ENGINE_EXPORT StringName
    {
    private:
        // Class
        uint32_t m_Hash;

        // Static string internment
        static std::deque<std::string> s_Strings; 
        static std::unordered_map<uint32_t, size_t> s_Registry; 

        static uint32_t GetID(std::string_view str);

    public:
        StringName(uint32_t hash) : m_Hash(hash) {}
        StringName(const std::string& string) : m_Hash(GetID(string)) {}
        StringName(const char* string) : m_Hash(GetID(string)) {}

        uint32_t GetHash() const { return m_Hash; }
        
        std::string_view GetString() const; 

        operator uint32_t() const { return m_Hash; }
        bool operator==(const StringName& other) const { return m_Hash == other.m_Hash; }
        bool operator==(const uint32_t& other) const { return m_Hash == other; }

        static void Reset();
        
    };
}

// Hack into hash
namespace std
{
    template<>
    struct hash<Engine::StringName>
    {
        std::size_t operator()(const Engine::StringName& sn) const noexcept
        {
            // Simply pass through your internal pre-computed hash
            return static_cast<std::size_t>(sn.GetHash());
        }
    };
} // namespace std


#endif // ENGINE_CORE_STRINGNAME
