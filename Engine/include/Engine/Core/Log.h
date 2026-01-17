#ifndef ENGINE_CORE_LOG
#define ENGINE_CORE_LOG

#include "engine_export.h"

#include <spdlog/spdlog.h>

namespace Engine
{
    class ENGINE_EXPORT Log
    {
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;

    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }
    };
    
} // namespace Engine

// Macros
#define LOG_CORE_TRACE(...)    ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)     ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)         ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)          ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)          ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)         ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)      ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif // ENGINE_CORE_LOG
