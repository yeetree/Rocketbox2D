#ifndef ENGINE_CORE_RESOURCEMANAGER
#define ENGINE_CORE_RESOURCEMANAGER

#include "engine_export.h"

#include "Engine/Renderer/IGraphicsDevice.h"

namespace Engine
{
    class ENGINE_EXPORT ResourceManager {
    public:
        ResourceManager(IGraphicsDevice* device) : m_GraphicsDevice(device) {}
        ResourceManager(const ResourceManager&) = delete; // No copying
        ~ResourceManager() = default;
        
        void LoadShader(const std::string& identifier, const std::string& vertPath, const std::string& fragPath);
        void LoadTexture(const std::string& identifier, const std::string& texturePath);

        std::shared_ptr<IShader> GetShader(const std::string& identifier);
        std::shared_ptr<ITexture> GetTexture(const std::string& identifier);


    private:
        // Graphics device for creating GPU resources
        IGraphicsDevice* m_GraphicsDevice;

        std::unordered_map<std::string, std::shared_ptr<IShader>> m_Shaders;
        std::unordered_map<std::string, std::shared_ptr<ITexture>> m_Textures;

    };
} // namespace Engine


#endif // ENGINE_CORE_RESOURCEMANAGER
