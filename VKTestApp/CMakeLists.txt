# EngineTestApp

# Create the executable
set(APP_SOURCES main.cpp)

engine_add_project(VKTestApp "${APP_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
add_slang_shader_target(VKTestAppShaders 
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.slang"
)
add_dependencies(VKTestApp VKTestAppShaders)