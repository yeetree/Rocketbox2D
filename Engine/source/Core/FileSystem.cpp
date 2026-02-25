#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

std::filesystem::path Engine::FileSystem::s_RootPath = "";

void Engine::FileSystem::SetBasePath(const std::string& basePath) {
    // Gets the absolute path of base path
    std::filesystem::path exePath = std::filesystem::absolute(basePath);
    s_RootPath = exePath;
}

std::vector<char> Engine::FileSystem::ReadFile(const std::string &path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOG_CORE_ERROR("Could not open file: {0}", FileSystem::GetAbsolutePath(path));
        return {};
    }

    std::streamsize fileSize = file.tellg();
    if (fileSize <= 0) {
        return {};
    }

    file.seekg(0, std::ios::beg);

    std::vector<char> buffer;
    buffer.resize(static_cast<size_t>(fileSize));

    if (!file.read(buffer.data(), fileSize)) {
        LOG_CORE_ERROR("Failed to read file: {0}", path);
        return {};
    }

    return buffer;
}

std::vector<uint32_t> Engine::FileSystem::ReadSPV(const std::string &path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOG_CORE_ERROR("Could not open SPIR-V file: {0}", path);
        return {};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());

    // SPIR-V files must be a multiple of 4 bytes
    if (fileSize == 0 || fileSize % 4 != 0) {
        LOG_CORE_ERROR("SPIR-V file is invalid or empty (size: {0}): {1}", fileSize, path);
        return {};
    }

    // Create buffer
    std::vector<uint32_t> buffer(fileSize / 4);

    // Read to uint32_t buffer
    file.seekg(0, std::ios::beg);
    if(!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        LOG_CORE_ERROR("Failed to read SPIR-V file: {0}", path);
        return {};
    }

    file.close();
    return buffer;
}

bool Engine::FileSystem::Exists(const std::string &path) {
    return std::filesystem::exists(path);
}

std::string Engine::FileSystem::GetAbsolutePath(const std::string& relativePath) {
    return (s_RootPath / relativePath).string();
}