#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <glm/ext/matrix_transform.hpp>
#include <cmath>

using namespace Engine;

const std::vector<float> vertices = {
    // Position     UV 
    -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f, -0.5f,   1.0f, 0.0f,
     0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f,   0.0f, 1.0f
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

// Push constant struct
struct PushData {
    alignas(16) glm::mat4 transform;
};

// Uniform constant struct
struct UniformData {
    alignas(16) glm::vec4 tint;
};

class EngineTestApp : public Application {
public:
    Ref<IBuffer> m_VertexBuffer;
    Ref<IBuffer> m_IndexBuffer;
    Ref<IUniformBuffer> m_UniformBuffer;
    Ref<IShader> m_Shader;
    Ref<ITexture> m_Texture;
    Ref<IPipelineState> m_Pipeline;

    PushData push;
    UniformData uni;

    void OnStart() override {
        std::vector<uint32_t> shaderCode = Engine::FileSystem::ReadSPV(FileSystem::GetAbsolutePath("./shaders/slang.spv"));
        ShaderDesc shaderDesc;
        shaderDesc.modules = {
            ShaderModule{
                .byteCode = shaderCode,
                .entryPoints = {
                    { ShaderStage::Vertex, "vertMain" },
                    { ShaderStage::Fragment, "fragMain" }
                }
            } 
        };
        
        m_Shader = GetGraphicsDevice().CreateShader(shaderDesc);

        PipelineDesc pipeDesc;
        pipeDesc.pushConstantSize = sizeof(PushData);
        pipeDesc.numUniformBuffers = 1;
        pipeDesc.numTextures = 1;
        pipeDesc.enableBlending = true;
        pipeDesc.shader = m_Shader.get();
        pipeDesc.layout = VertexLayout{VertexElement(VertexElementType::Vec2, "inPosition"), VertexElement(VertexElementType::Vec2, "inCoord")};

        m_Pipeline =  GetGraphicsDevice().CreatePipelineState(pipeDesc);

        BufferDesc vbDesc;
        vbDesc.data = vertices.data();
        vbDesc.size = sizeof(vertices[0]) * vertices.size();
        vbDesc.type = BufferType::Vertex;

        m_VertexBuffer = GetGraphicsDevice().CreateBuffer(vbDesc);

        BufferDesc ibDesc;
        ibDesc.data = indices.data();
        ibDesc.size = sizeof(indices[0]) * indices.size();
        ibDesc.type = BufferType::Index;

        m_IndexBuffer = GetGraphicsDevice().CreateBuffer(ibDesc);

        UniformBufferDesc ubDesc;
        ubDesc.size = sizeof(UniformData);
        ubDesc.data = &uni;
        // Need a CreateUniformBuffer or similar in your GraphicsDevice
        m_UniformBuffer = GetGraphicsDevice().CreateUniformBuffer(ubDesc);

        GetResourceManager().LoadTexture("awesome", "./Assets/awesomeface.png");
        m_Texture = GetResourceManager().GetTexture("awesome");

        GetGraphicsDevice().SetClearColor(Vec4(0.0f, 0.0f, 0.5f, 1.0f));
    }

    void OnInput(SDL_Event event) override {

    }

    void OnUpdate(float dt) override {
        push.transform = glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0, 0, 1));
        uni.tint = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    }

    void OnRender() override {
        GetGraphicsDevice().BindPipelineState(*m_Pipeline);
        GetGraphicsDevice().BindUniformBuffer(*m_UniformBuffer, 0);
        m_UniformBuffer->UpdateData(&uni, sizeof(UniformData), 0);
        GetGraphicsDevice().PushConstants(&push, sizeof(PushData));
        GetGraphicsDevice().BindTexture(*m_Texture, 1);
        GetGraphicsDevice().SubmitDraw(*m_VertexBuffer, *m_IndexBuffer, indices.size());
    }

    void OnDestroy() override {
        // Manually destroy our objects because this class technically outlives the rest
        // of the engine and vulkan is NOT happy with us.
        LOG_TRACE("Client destroying...");
        m_Pipeline.reset();
        m_Shader.reset();
        m_VertexBuffer.reset();
        m_IndexBuffer.reset();
        m_UniformBuffer.reset();
        LOG_TRACE("Client done destroying.");
    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "VKTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}