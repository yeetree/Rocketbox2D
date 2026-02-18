# EngineTestApp

# Create the executable
set(APP_SOURCES main.cpp)

add_slang_shader(VKTestApp "./shaders/test.slang" "./shaders/slang.spv" "vertMain" "fragMain")
engine_add_project(VKTestApp "${APP_SOURCES}" "./Assets" "./Assets")