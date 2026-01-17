#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

std::filesystem::path Engine::FileSystem::s_RootPath = "";

void Engine::FileSystem::SetBasePath(const char* basePath) {
    // Gets the absolute path of base path
    std::filesystem::path exePath = std::filesystem::absolute(basePath);
    s_RootPath = exePath;
}

std::string Engine::FileSystem::ReadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_CORE_ERROR("Could not open file! File not found: {0}", FileSystem::GetAbsolutePath(path));
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Engine::FileSystem::Exists(const std::string &path) {
    return std::filesystem::exists(path);
}

std::string Engine::FileSystem::GetAbsolutePath(const std::string& relativePath) {
    return (s_RootPath / relativePath).string();
}