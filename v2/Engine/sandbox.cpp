#include "engine.hpp"
#include <iostream>

#include "imgui.h"

#include "Engine/application.hpp"
#include "Engine/layer.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"
#include "Engine/renderer/renderer.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/entity.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace engine;

class MyLayer: public engine::Layer {
public:
    MyLayer() {
        std::string vs_file = "./shaders/vertex.glsl";
        std::string fs_file = "./shaders/fragment.glsl";
        shader.reset(new Shader{vs_file, fs_file});

        camera.reset(new OrthographicCamera {-2.0f, 2.0f, -2.0f, 2.0f} );

        std::vector<glm::vec4> data = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            { 0.5f, -0.5f, 0.0f, 1.0f },
            { 0.0f,  0.5f, 0.0f, 1.0f }
        };
        std::vector<uint32_t> i_data = { 0, 1, 2 };

        entity.add_attribute_data("position", data);
        entity.add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));
        entity.add_index_data(i_data);
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

        shader->on_ui_render(true);
    }

    void on_update() override {
        {
            float time = (float)glfwGetTime();
            float delta_time = time - last_frame_time;
            last_frame_time = time;

            auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
            int state = glfwGetKey(window, GLFW_KEY_W);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                camera_position.y += camera_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_S);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                camera_position.y -= camera_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_A);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                camera_position.x -= camera_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_D);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                camera_position.x += camera_move_speed * delta_time;
            }
            camera->set_position(camera_position);

            state = glfwGetKey(window, GLFW_KEY_UP);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                model_position.y += model_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_DOWN);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                model_position.y -= model_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_LEFT);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                model_position.x -= model_move_speed * delta_time;
            }
            state = glfwGetKey(window, GLFW_KEY_RIGHT);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                model_position.x += model_move_speed * delta_time;
            }

            state = glfwGetKey(window, GLFW_KEY_X);
            if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                shader->recompile();
            }
        }

        Renderer::begin_scene(camera, glm::vec4{0.5f, 0.5f, 0.5f, 1.0f});
        entity.add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));
        Renderer::submit_entity(shader, entity);
    }

private:
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> buffer; 
    std::shared_ptr<IndexBuffer> index_buffer;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Camera> camera;
    Entity entity;
    glm::mat4 model_matrix {1.0f};
    glm::vec3 model_position {0.0f};
    glm::vec3 camera_position {0.0f};
    float camera_move_speed = 5.0f;
    float model_move_speed = 2.0f;
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;
};

int main() {
    engine::Application application;
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
