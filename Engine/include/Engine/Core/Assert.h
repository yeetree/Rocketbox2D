#ifndef ENGINE_CORE_ASSERT_H
#define ENGINE_CORE_ASSERT_H

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"
#include <filesystem>

// From https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/Assert.h
#ifdef ENGINE_ENABLE_ASSERTS
	#define ENGINE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { LOG##type##ERROR(msg, __VA_ARGS__); ENGINE_DEBUGBREAK(); } }
	#define ENGINE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ENGINE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ENGINE_INTERNAL_ASSERT_NO_MSG(type, check) ENGINE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ENGINE_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ENGINE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ENGINE_INTERNAL_ASSERT_GET_MACRO(...) ENGINE_EXPAND_MACRO( ENGINE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ENGINE_INTERNAL_ASSERT_WITH_MSG, ENGINE_INTERNAL_ASSERT_NO_MSG) )

	// Params:
    //  arg1: condition: if condition is false, debug break;
    //  arg2: message (optional): message to display if assertation fails
	#define ENGINE_ASSERT(...) ENGINE_EXPAND_MACRO( ENGINE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ENGINE_CORE_ASSERT(...) ENGINE_EXPAND_MACRO( ENGINE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ENGINE_ASSERT(...)
	#define ENGINE_CORE_ASSERT(...)
#endif

#endif // ENGINE_CORE_ASSERT_H
