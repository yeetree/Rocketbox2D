#ifndef ENGINE_CORE_FILESYSTEM
#define ENGINE_CORE_FILESYSTEM

#include "engine_export.h"

#include <string>
#include <filesystem>

namespace Engine
{
    class ENGINE_EXPORT FileSystem {
    public:
        static void SetBasePath(const char* basePath);
        static std::string ReadFile(const std::string &path);
        static bool Exists(const std::string &path);
        static std::string GetAbsolutePath(const std::string& relativePath);
    private:
        static std::filesystem::path s_RootPath;
    };
} // namespace Engine


#endif // ENGINE_CORE_FILESYSTEM
