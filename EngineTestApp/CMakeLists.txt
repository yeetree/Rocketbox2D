# EngineTestApp

# Create the executable
add_executable(EngineTestApp main.cpp)

# Link the engine library and include public headers
target_link_libraries(EngineTestApp PRIVATE Engine::Engine)

# Copy assets
add_custom_command(TARGET EngineTestApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:EngineTestApp>/Assets"
    COMMENT "Copying assets to build directory..."
)