#ifndef ENGINE_ENGINE
#define ENGINE_ENGINE

#include "Engine/Core/EntryPoint.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/Base.h"

#include "Engine/Resources/IResource.h"
#include "Engine/Resources/IResourceLoader.h"
#include "Engine/Resources/ResourceManager.h"

#include "Engine/Platform/IWindow.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/EventManager.h"

#include "Engine/Input/KeyCode.h"
#include "Engine/Input/InputAxis.h"
#include "Engine/Input/Input.h"

#include "Engine/RHI/RHI.h"
#include "Engine/RHI/IGraphicsDevice.h"
#include "Engine/RHI/ICommandBuffer.h"

/*
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Renderer/RHI/IPipelineState.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/VertexLayout.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Camera.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
*/

#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

#endif // ENGINE_ENGINE
