#ifndef ENGINE_CORE_FILESYSTEM
#define ENGINE_CORE_FILESYSTEM

#include "engine_export.h"

#include <string>
#include <vector>
#include <filesystem>

namespace Engine
{
    class ENGINE_EXPORT FileSystem {
    public:
        FileSystem(const std::string& basePath = "");
        ~FileSystem();

        void SetBasePath(const std::string& basePath);
        std::vector<char> ReadFile(const std::string &path);
        std::vector<uint32_t> ReadSPV(const std::string &path);
        bool Exists(const std::string &path);
        std::string GetAbsolutePath(const std::string& relativePath);
    private:
        std::filesystem::path s_BasePath;
    };
} // namespace Engine


#endif // ENGINE_CORE_FILESYSTEM
