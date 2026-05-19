#ifndef ENGINE_EVENTS_EVENT
#define ENGINE_EVENTS_EVENT

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Core/Hash.h"

#include <string>
#include <cstdint>

namespace Engine {

    using EventType = uint32_t; 

    class ENGINE_EXPORT Event {
    private:
        EventType m_Type;

    public:
        virtual ~Event() = default;
    };

} // namespace Engine

#endif // ENGINE_EVENTS_EVENT