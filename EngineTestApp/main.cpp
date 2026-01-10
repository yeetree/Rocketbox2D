#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>

const char * vert = 
"#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";

const char * frag = 
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"   color = vec4(1.0, 0.0, 0.0, 1.0);"
"}";

std::vector<float> vertsData = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.5f
};

std::vector<unsigned int> indicesData = {
    0, 1, 2
};

using namespace Engine;

class EngineTestApp : public Engine {
public:
    std::unique_ptr<IPipelineState> pipeline;
    std::unique_ptr<IShader> shader;
    std::unique_ptr<IBuffer> verts;
    std::unique_ptr<IBuffer> indices;

    void Startup() override {
        shader = GetGraphicsDevice().CreateShader(ShaderDesc{
            {
                {ShaderStage::Vertex, vert},
                {ShaderStage::Fragment, frag},
            }
        });

        verts = GetGraphicsDevice().CreateBuffer(BufferDesc{
            vertsData.size() * sizeof(float),
            BufferType::Vertex,
            vertsData.data()
        });

        indices = GetGraphicsDevice().CreateBuffer(BufferDesc{
            indicesData.size() * sizeof(unsigned int),
            BufferType::Index,
            indicesData.data()
        });

        pipeline = GetGraphicsDevice().CreatePipelineState(PipelineDesc{
            shader.get(),
            VertexLayout({VertexElement(VertexElementType::Vec2, "position")}),
        });
    }

    void Input(SDL_Event event) override {

    }

    void Update() override {

    }

    void Render() override {
        pipeline->Bind();
        verts->Bind();
        indices->Bind();
        GetGraphicsDevice().SubmitDraw(indicesData.size());
    }

    void Cleanup() override {

    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "EngineTestApp", NULL);
    app.Run();
    return 0;
}