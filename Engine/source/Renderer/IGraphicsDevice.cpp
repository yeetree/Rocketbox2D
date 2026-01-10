#include "Engine/Renderer/IGraphicsDevice.h"
#include "Renderer/OpenGL/OpenGLGraphicsDevice.h"

namespace Engine
{
    std::unique_ptr<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api, SDL_Window* window) {
        switch (api) {
            case GraphicsAPI::OpenGL: return std::make_unique<OpenGLGraphicsDevice>(window);
        }
        return nullptr;
    }
} // namespace Engine
