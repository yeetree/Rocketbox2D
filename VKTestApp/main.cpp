#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <glm/ext/matrix_transform.hpp>
#include <cmath>

// Generated
#include "generated/quadShader.h"

// Quad data
const float quadVerts[] = {
    -0.5f,  -0.5f,   0.0f,  0.0f, // Bottom Left
    0.5f,   -0.5f,  1.0f,   0.0f, // Bottom Right
    0.5f,   0.5f,   1.0f,   1.0f, // Top Right
    -0.5f,  0.5f,   0.0f,   1.0   // Top Left
};

const uint16_t quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

using namespace Engine;

class EngineTestApp : public Application {
public:

    Ref<ITexture> texture;
    Camera cam;
    Mat4 transformMat;
    Vec4 tint;

    Ref<IBuffer> vbo, ebo;
    UniformBlock global, local;

    Ref<IShader> shader;
    Ref<IPipelineState> pso;

    void OnStart() override {

        BufferDesc vboDesc, eboDesc;
        
        // Create VBO and EBO for quad
        vboDesc.data = quadVerts;
        vboDesc.isDynamic = false;
        vboDesc.size = sizeof(quadVerts);
        vboDesc.type = BufferType::Vertex;

        eboDesc.data = quadIndices;
        eboDesc.isDynamic = false;
        eboDesc.size = sizeof(quadIndices);
        eboDesc.type = BufferType::Index;

        vbo = GetGraphicsDevice().CreateBuffer(vboDesc);
        ebo = GetGraphicsDevice().CreateBuffer(eboDesc);

        // Create shader
        ShaderDesc shaderDesc;

        // Listen, i PROMISE that k_QuadShaderByteCode_len is divisible by 4
        const uint32_t* shaderData = static_cast<const uint32_t*>(static_cast<const void*>(k_QuadShaderByteCode));
        std::vector<uint32_t> shaderDataVector(shaderData, shaderData + k_QuadShaderByteCode_len / sizeof(uint32_t));
        shaderDesc.modules = {
            ShaderModule{
                .byteCode = shaderDataVector,
                .entryPoints = {
                    {ShaderStage::Vertex, "vertMain"},
                    {ShaderStage::Fragment, "fragMain"}
                }
            }
        };
        shader = GetGraphicsDevice().CreateShader(shaderDesc);

        // shader layout
        ShaderLayout shaderLayout = ShaderLayout{
            // Global: Binding 0, Set 0
            ShaderBinding(
                ShaderBindingType::UniformBuffer,"global", 0, 0, 0, 
                { ShaderElement(ShaderDataType::Mat4, "viewProjection") }),
            
            // Local: Binding 0, Set 1
            ShaderBinding(
                ShaderBindingType::UniformBuffer, "local", 1, 0, 1, 
                { ShaderElement(ShaderDataType::Mat4, "transform"), 
                ShaderElement(ShaderDataType::Vec4, "tint") }),
            
            // Texture: Binding 1, Set 1
            ShaderBinding(ShaderBindingType::Sampler, "texture", 2, 1, 1)
        };

        // Vert layout
        VertexLayout vertLayout = VertexLayout{VertexElement(VertexElementType::Vec2, "inPosition"), VertexElement(VertexElementType::Vec2, "inCoord")};

        PipelineDesc psoDesc;
        psoDesc.shader = shader.get();
        psoDesc.shaderLayout = shaderLayout;
        psoDesc.vertexLayout = vertLayout;
        psoDesc.pushConstantSize = 0;
        psoDesc.topology = PrimitiveTopology::TriangleList;
        psoDesc.fillMode = FillMode::Fill;
        psoDesc.cullMode = CullMode::None;
        psoDesc.enableBlending = true;

        pso = GetGraphicsDevice().CreatePipelineState(psoDesc);

        global = UniformBlock(*shaderLayout.GetBinding("global"));
        local = UniformBlock(*shaderLayout.GetBinding("local"));

        GetResourceManager().LoadTexture("awesome", "./Assets/awesomeface.png");
        texture = GetResourceManager().GetTexture("awesome");
        cam = Camera(800.0f);
        cam.OnResize();

        GetGraphicsDevice().SetClearColor(Vec4(1.0, 0.0, 0.0, 1.0));
    }

    void OnInput(SDL_Event event) override {

    }

    void OnUpdate(float dt) override {
        transformMat = glm::translate(glm::mat4(1.0f), Vec3(0.0f, 0.0f, 0.0f)); 
        transformMat = glm::scale(transformMat, Vec3(100.0f, 100.0f, 1.0f));
    
        tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        global.Set("viewProjection", cam.GetViewProjectionMatrix());
        local.Set("transform", transformMat);
        local.Set("tint", tint);
        global.Upload(&GetGraphicsDevice());
        local.Upload(&GetGraphicsDevice());
    }

    void OnRender() override {
        GetGraphicsDevice().BindPipelineState(*pso);
        GetGraphicsDevice().BindUniformBuffer(*global.GetUniformBuffer(), 0, 0);
        GetGraphicsDevice().BindUniformBuffer(*local.GetUniformBuffer(), 0, 1);
        GetGraphicsDevice().BindTexture(*texture, 1, 1);
        GetGraphicsDevice().SubmitDraw(*vbo, *ebo, 6);
    }

    void OnDestroy() override {

    }
};

int Engine::EntryPoint(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "VKTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}