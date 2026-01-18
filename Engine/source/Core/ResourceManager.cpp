#include "Engine/Core/ResourceManager.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
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

        // Get pixel data
        int width, height;
        unsigned char* pixels = stbi_load(FileSystem::GetAbsolutePath(texturePath).c_str(), &width, &height, nullptr, 4);

        desc.data = pixels;
        desc.width = width;
        desc.height = height;
        desc.format = TextureFormat::RGBA8;
        desc.usage = TextureUsage::Sampled;

        if(pixels == nullptr)
        {
            LOG_CORE_ERROR("Could not load image! File not found: {0}", FileSystem::GetAbsolutePath(texturePath));
            return;
        }

        // Get texture (unique)
        std::unique_ptr<ITexture> texturePtr = m_GraphicsDevice->CreateTexture(desc);

        // Free pixels
        stbi_image_free(pixels);

        // Move it into a shared_ptr and store in map
        std::shared_ptr<ITexture> sharedTexture = std::move(texturePtr);
        m_Textures[identifier] = sharedTexture;
    }

    void ResourceManager::CreateMesh(const std::string& identifier, const void* vertices, uint32_t vSize, const void* indices, uint32_t iSize, uint32_t indexCount, const VertexLayout& layout, bool vDynamic, bool iDynamic) {
        BufferDesc vboDesc, eboDesc;
        vboDesc.data = vertices;
        eboDesc.data = indices;
        vboDesc.size = vSize;
        eboDesc.size = iSize;
        vboDesc.type = BufferType::Vertex;
        eboDesc.type = BufferType::Index;
        vboDesc.isDynamic = vDynamic;
        eboDesc.isDynamic = iDynamic;
        std::shared_ptr<IBuffer> vbo = m_GraphicsDevice->CreateBuffer(vboDesc);
        std::shared_ptr<IBuffer> ebo = m_GraphicsDevice->CreateBuffer(eboDesc);

        VertexArrayDesc vaoDesc;
        vaoDesc.vbo = vbo;
        vaoDesc.ebo = ebo;
        vaoDesc.layout = layout;

        std::shared_ptr<IVertexArray> vao = m_GraphicsDevice->CreateVertexArray(vaoDesc);
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vao, indexCount, layout);
        m_Meshes[identifier] = mesh;
    }

    void ResourceManager::CreateMaterial(const std::string& identifier, std::shared_ptr<IShader> shader) {
        if(!shader) {
            LOG_CORE_WARN("Resource Warning: Material: {0} was created with a null shader!", identifier);
        }

        std::shared_ptr<Material> material = std::make_shared<Material>(shader);
        m_Materials[identifier] = material;
    }

    void ResourceManager::SetShader(const std::string& identifier, std::unique_ptr<IShader> shader) {
        m_Shaders[identifier] = std::move(shader);
    }

    void ResourceManager::SetTexture(const std::string& identifier, std::unique_ptr<ITexture> texture) {
        m_Textures[identifier] = std::move(texture);
    }

    void ResourceManager::SetMesh(const std::string& identifier, std::unique_ptr<Mesh> mesh) {
        m_Meshes[identifier] = std::move(mesh);
    }

    void ResourceManager::SetMaterial(const std::string& identifier, std::unique_ptr<Material> material) {
        m_Materials[identifier] = std::move(material);
    }

    std::shared_ptr<IShader> ResourceManager::GetShader(const std::string& identifier) {
        if (m_Shaders.count(identifier)) {
            if (auto ptr = m_Shaders[identifier]) {
                return ptr;
            }
        }
        LOG_CORE_ERROR("Resource Error: Shader {0} not found!", identifier);
        return nullptr;
    }

    std::shared_ptr<ITexture> ResourceManager::GetTexture(const std::string& identifier) {
        if (m_Textures.count(identifier)) {
            if (auto ptr = m_Textures[identifier]) {
                return ptr;
            }
        }
        LOG_CORE_ERROR("Resource Error: Texture {0} not found!", identifier);
        return nullptr;
    }

    std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& identifier){
        if (m_Meshes.count(identifier)) {
            if (auto ptr = m_Meshes[identifier]) {
                return ptr;
            }
        }
        LOG_CORE_ERROR("Resource Error: Mesh {0} not found!", identifier);
        return nullptr;
    }

    std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& identifier){
        if (m_Materials.count(identifier)) {
            if (auto ptr = m_Materials[identifier]) {
                return ptr;
            }
        }
        LOG_CORE_ERROR("Resource Error: Material {0} not found!", identifier);
        return nullptr;
    }
} // namespace Engine
