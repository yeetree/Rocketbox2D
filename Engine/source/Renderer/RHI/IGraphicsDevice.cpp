#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Renderer/OpenGL/OpenGLGraphicsDevice.h"

namespace Engine
{
    // Static function to create GraphicsDevice with selected graphics api
    std::unique_ptr<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api, SDL_Window* window) {
        switch (api) {
            case GraphicsAPI::OpenGL: return std::make_unique<OpenGLGraphicsDevice>(window);
        }
        return nullptr;
    }
} // namespace Engine
