# EngineTestApp

# Create the executable
set(APP_SOURCES main.cpp)

engine_add_project(CoreTestApp "${APP_SOURCES}" "./Assets" "./Assets")
add_slang_shader(CoreTestApp "./Assets/Shaders/shader.slang" "./Assets/Shaders/shader.spv" "vertMain" "fragMain")