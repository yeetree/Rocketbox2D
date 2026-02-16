#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <cmath>

using namespace Engine;

const std::vector<float> vertices = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 1.0f, 1.0f, 1.0f
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

class EngineTestApp : public Application {
public:
    Ref<IShader> m_Shader;
    Ref<IBuffer> m_VertexBuffer;
    Ref<IBuffer> m_IndexBuffer;
    Ref<IPipelineState> m_Pipeline;

    void OnStart() override {
       std::vector<char> shaderCode = Engine::FileSystem::ReadFile(FileSystem::GetAbsolutePath("./shaders/slang.spv"));
        ShaderDesc shaderDesc;
        shaderDesc.stages[ShaderStage::Vertex].byteCode = shaderCode;
        shaderDesc.stages[ShaderStage::Vertex].entryPoint = "vertMain";
        shaderDesc.stages[ShaderStage::Fragment].byteCode = shaderCode;
        shaderDesc.stages[ShaderStage::Fragment].entryPoint = "fragMain";
        
        m_Shader = GetGraphicsDevice().CreateShader(shaderDesc);

        PipelineDesc pipeDesc;
        pipeDesc.shader = m_Shader.get();
        pipeDesc.layout = VertexLayout{VertexElement(VertexElementType::Vec2, "inPosition"), VertexElement(VertexElementType::Vec3, "inColor")};

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
    }

    void OnInput(SDL_Event event) override {

    }

    void OnUpdate(float dt) override {

    }

    void OnRender() override {
        GetGraphicsDevice().SubmitDraw(*m_VertexBuffer, *m_IndexBuffer, *m_Pipeline, indices.size());
    }

    void OnDestroy() override {

    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "VKTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}