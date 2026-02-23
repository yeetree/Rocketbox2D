#ifndef ENGINE_CORE_PLATFORMDETECTION
#define ENGINE_CORE_PLATFORMDETECTION

// Based on https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/PlatformDetection.h

#ifdef _WIN64
    #define ENGINE_PLATFORM_WINDOWS
#elif defined(__linux__) && !defined(__ANDROID__)
    #define ENGINE_PLATFORM_LINUX
#else
    #error "Platform is not supported!"
#endif

#endif // ENGINE_CORE_PLATFORMDETECTION
