#ifndef ENGINE_CORE_TIMER
#define ENGINE_CORE_TIMER

#include "engine_export.h"

#include <chrono>

namespace Engine
{
    class ENGINE_EXPORT Timer
    {
    public:
        Timer();

        void Reset();

        float GetTime() const;
        float GetTimeMS() const;
        double DGetTime() const;
        double DGetTimeMS() const;

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };
} // namespace Engine


#endif // ENGINE_CORE_TIMER
