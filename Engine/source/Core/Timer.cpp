#include "Engine/Core/Timer.h"

namespace Engine
{
    Timer::Timer()
    {
        Reset();
    }

    void Timer::Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Timer::GetTime() const
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - m_Start;
        return elapsed.count();
    }

    float Timer::GetTimeMS() const
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> elapsed = now - m_Start;
        return elapsed.count();
    }

    double Timer::DGetTime() const
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - m_Start;
        return elapsed.count();
    }

    double Timer::DGetTimeMS() const
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = now - m_Start;
        return elapsed.count();
    }

} // namespace Engine
