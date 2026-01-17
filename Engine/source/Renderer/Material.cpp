#include "Engine/Renderer/Material.h"
#include "Engine/Core/Application.h"

namespace Engine
{
    Material::Material(std::shared_ptr<IShader> shader) : m_Shader(shader) {

    }

} // namespace Engine
