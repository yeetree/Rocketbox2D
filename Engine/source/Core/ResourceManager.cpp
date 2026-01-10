#include "Engine/Core/ResourceManager.h"
#include "Engine/Core/FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

namespace Engine
{
    void ResourceManager::LoadShader(const std::string& identifier, const std::string& vertPath, const std::string& fragPath) {

        std::string vertSource = FileSystem::ReadFile(FileSystem::GetAbsolutePath(vertPath));
        std::string fragSource = FileSystem::ReadFile(FileSystem::GetAbsolutePath(fragPath));

        ShaderDesc desc;
        desc.sources[ShaderStage::Vertex] = vertSource;
        desc.sources[ShaderStage::Fragment] = fragSource;

        // Get shader (unique)
        std::unique_ptr<IShader> shaderPtr = m_GraphicsDevice->CreateShader(desc);

        // Move it into a shared_ptr and store in map
        std::shared_ptr<IShader> sharedShader = std::move(shaderPtr);
        m_Shaders[identifier] = sharedShader;
    }

    void ResourceManager::LoadTexture(const std::string& identifier, const std::string& texturePath) {
        TextureDesc desc;

        stbi_set_flip_vertically_on_load(1);
        int width, height;
        desc.data = stbi_load(FileSystem::GetAbsolutePath(texturePath).c_str(), &width, &height, nullptr, 4);

        desc.format = TextureFormat::RGBA8;
        desc.usage = TextureUsage::Sampled;

        if(desc.data == nullptr)
        {
            std::cout << "Couldn't load image: " << FileSystem::GetAbsolutePath(texturePath) << std::endl;
        }

        // Get texture (unique)
        std::unique_ptr<ITexture> texturePtr = m_GraphicsDevice->CreateTexture(desc);

        // Move it into a shared_ptr and store in map
        std::shared_ptr<ITexture> sharedTexture = std::move(texturePtr);
        m_Textures[identifier] = sharedTexture;
    }

    std::shared_ptr<IShader> ResourceManager::GetShader(const std::string& identifier) {
        if (m_Shaders.count(identifier)) {
            if (auto ptr = m_Shaders[identifier]) {
                return ptr;
            }
        }
        std::cout << "Resource Error: Shader " << identifier << " not found!" << std::endl;
        return nullptr;
    }

    std::shared_ptr<ITexture> ResourceManager::GetTexture(const std::string& identifier) {
        if (m_Textures.count(identifier)) {
            if (auto ptr = m_Textures[identifier]) {
                return ptr;
            }
        }
        std::cout << "Resource Error: Texture " << identifier << " not found!" << std::endl;
        return nullptr;
    }
} // namespace Engine
