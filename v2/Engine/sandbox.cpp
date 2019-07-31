#include "engine.hpp"
#include <iostream>

#include "imgui.h"

#include "Engine/application.hpp"
#include "Engine/layer.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"
#include "Engine/renderer/renderer.hpp"

using namespace engine;

class MyLayer: public engine::Layer {
public:
    MyLayer() {

        std::string vs_file = "./shaders/vertex.glsl";
        std::string fs_file = "./shaders/fragment.glsl";
        shader.reset(new Shader{vs_file, fs_file});

        vao.reset(VertexArray::create());

        float vertices[3 * 3] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };

        BufferLayout layout {
            {ShaderDataType::Float3, "position"}
        };
        
        buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
        buffer->set_layout(layout);

        uint32_t indices[3] = { 0, 1, 2 };
        
        index_buffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

        vao->add_vertex_buffer(buffer);
        vao->set_index_buffer(index_buffer);
    }

    void on_attach() override {
        GAME_INFO("My layer attached");
    }

    void on_ui_render() override {
        static int counter = 0;
        ImGui::Begin("Test");
        ImGui::Text("Sandbox UI");
        if (ImGui::Button("Count")){
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Count: %i", counter);
        ImGui::End();
    }

    void on_update() override {
        Renderer::begin_scene(0.5f, 0.5f, 0.5f, 1.0f);
        shader->bind();
        Renderer::submit(vao);
        shader->unbind();
    }

private:
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> buffer;
    std::shared_ptr<IndexBuffer> index_buffer;
    std::shared_ptr<Shader> shader;
};

int main() {
    engine::Application application;
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
