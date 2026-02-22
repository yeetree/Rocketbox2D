#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
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
    Mat4 viewProjMat, transformMat;
    Vec4 tint;

    UniformBlock globalU, localU;
    Ref<IBuffer> vbo, ebo, globalB, localB;

    Ref<IShader> shader;
    Ref<IPipelineState> pso;

    void OnStart() override {
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
                { 
                    ShaderElement(ShaderDataType::Mat4, "viewProjection")
                }
            ),
            
            // Local: Binding 0, Set 1
            ShaderBinding(
                ShaderBindingType::UniformBuffer, "local", 1, 0, 1, 
                { 
                    ShaderElement(ShaderDataType::Mat4, "transform"), 
                    ShaderElement(ShaderDataType::Vec4, "tint") 
                }
            ),
            
            // Texture: Binding 1, Set 1
            ShaderBinding(ShaderBindingType::Sampler, "texture", 2, 1, 1)
        };

        // uniform data mapper
        globalU = UniformBlock(*shaderLayout.GetBindingBySlot(0));
        localU = UniformBlock(*shaderLayout.GetBindingBySlot(1));

        // set uniforms
        transformMat = glm::translate(glm::mat4(1.0f), Vec3(0.5f, 0.0f, 0.0f)); 
        transformMat = glm::scale(transformMat, Vec3(100.0f, 100.0f, 1.0f)); 

        cam = Camera(600);
        cam.OnResize();

        globalU.Set("viewProjection", cam.GetViewProjectionMatrix());
        localU.Set("transform", transformMat);
        localU.Set("tint", Vec4(1.0f, 1.0f, 1.0f, 1.0f));

        BufferDesc vboDesc, eboDesc, globalDesc, localDesc;
        
        // Create VBO, EBO, and UBOs for quad
        vboDesc.data = quadVerts;
        vboDesc.isDynamic = false;
        vboDesc.size = sizeof(quadVerts);
        vboDesc.type = BufferType::Vertex;

        eboDesc.data = quadIndices;
        eboDesc.isDynamic = false;
        eboDesc.size = sizeof(quadIndices);
        eboDesc.type = BufferType::Index;

        globalDesc.data = globalU.GetData();
        globalDesc.size = globalU.GetSize();
        globalDesc.isDynamic = true;
        globalDesc.type = BufferType::Uniform;

        localDesc.data = localU.GetData();
        localDesc.size = localU.GetSize();
        localDesc.isDynamic = true;
        localDesc.type = BufferType::Uniform;

        vbo = GetGraphicsDevice().CreateBuffer(vboDesc);
        ebo = GetGraphicsDevice().CreateBuffer(eboDesc);
        globalB = GetGraphicsDevice().CreateBuffer(globalDesc);
        localB = GetGraphicsDevice().CreateBuffer(localDesc);

        // Vert layout
        LOG_CORE_TRACE("Create init vert layout");
        VertexLayout vertLayout({
            VertexElement(VertexElementType::Vec2, "inPosition"),
            VertexElement(VertexElementType::Vec2, "inCoords")
        });
        LOG_CORE_TRACE("End Create init vert layout");

        PipelineDesc psoDesc{
            .shader = shader.get(),
            .shaderLayout = shaderLayout,
            .pushConstantSize = 0,
            .vertexLayout = vertLayout,
            .topology = PrimitiveTopology::TriangleList,
            .fillMode = FillMode::Fill,
            .cullMode = CullMode::None,
            .enableBlending = true
        };

        pso = GetGraphicsDevice().CreatePipelineState(psoDesc);

        GetResourceManager().LoadTexture("awesome", "./Assets/awesomeface.png");
        texture = GetResourceManager().GetTexture("awesome");

        GetGraphicsDevice().SetClearColor(Vec4(1.0, 0.0, 0.0, 1.0));
    }

    void OnInput(SDL_Event event) override {

    }

    void OnUpdate(float dt) override {
        globalU.Set("viewProjection", cam.GetViewProjectionMatrix());
        localU.Set("transform", transformMat);
        localU.Set("tint", Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        globalB->UpdateData(globalU.GetData(), globalU.GetSize(), 0);
        localB->UpdateData(localU.GetData(), localU.GetSize(), 0);
    }

    void OnRender() override {
        GetGraphicsDevice().BindPipelineState(*pso);
        GetGraphicsDevice().BindUniformBuffer(*globalB, 0, 0);
        GetGraphicsDevice().BindUniformBuffer(*localB, 0, 1);
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