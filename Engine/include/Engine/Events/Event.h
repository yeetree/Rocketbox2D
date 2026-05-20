#ifndef ENGINE_EVENTS_EVENT
#define ENGINE_EVENTS_EVENT

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Core/StringName.h"

#include <string>
#include <cstdint>

namespace Engine {
    class ENGINE_EXPORT Event {
    private:
        StringName m_Type;

    public:
        Event(const StringName& type) : m_Type(type) {};
        virtual ~Event() = default;
    };

} // namespace Engine

#endif // ENGINE_EVENTS_EVENT
