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
#include "Tools/gltf_loader.hpp"
#include "Engine/renderer/texture.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Platform/OpenGL/gl_debug_ui.hpp"


using namespace engine;

class MyLayer: public engine::Layer {
public:
    MyLayer() {

        #ifdef OPENGL_COMPATIBILITY
        std::string vs_file_color = "./shaders/opengl2_vertex.glsl";
        std::string fs_file_color = "./shaders/opengl2_fragment.glsl";

        std::string vs_file_texture = "./shaders/opengl2_vertex_texture.glsl";
        std::string fs_file_texture = "./shaders/opengl2_fragment_texture.glsl";
        #else
        std::string vs_file = "./shaders/vertex.glsl";
        std::string fs_file = "./shaders/fragment.glsl";
        #endif

        texture_shader.reset(new Shader{vs_file_texture, fs_file_texture});
        color_shader.reset(new Shader{vs_file_color, fs_file_color});

        //camera.reset(new OrthographicCamera {-2.0f, 2.0f, -2.0f, 2.0f} );
        float aspect = 800/800;
        camera.reset(new PerspectiveCamera { 45.0f, aspect, 0.1f, 100.0f });

        camera->set_position(glm::vec3(3.0f, 3.0f, 10.0f));

        // TODO: Connect mesh index information to the rendered::submit call
        // It requires object count and data type (uint/ushort)
        gearbox = GltfEntity::load_from_file("./assets/gltf/GearboxAssy.gltf");
        cube = GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");
        monkey = GltfEntity::load_from_file("./assets/gltf/Monkey/monkey.gltf");
        square.reset( new CustomEntity());
        
        std::vector<glm::vec4> data = {
            {  0.5f,  0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            { -0.5f, -0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };
        std::vector<glm::vec4> colors = {
            { 0.8f, 0.2f, 0.2f, 1.0f },
            { 0.2f, 0.8f, 0.2f, 1.0f },
            { 0.2f, 0.2f, 0.8f, 1.0f },
            { 0.2f, 0.2f, 0.2f, 1.0f }
        };
        std::vector<glm::vec4> normals = {
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f }
        };
        std::vector<glm::vec2> texcoords = {
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 1.0f }
        };

        std::vector<uint32_t> i_data = { 0, 1, 3, 1, 2, 3 };

        std::static_pointer_cast<CustomEntity>(square)->add_attribute_data("position", data);
        std::static_pointer_cast<CustomEntity>(square)->add_attribute_data("normal", normals);
        std::static_pointer_cast<CustomEntity>(square)->add_attribute_data("texcoord", texcoords);
        std::static_pointer_cast<CustomEntity>(square)->add_index_data(i_data);

        square->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));
        square->add_uniform_data("u_color", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

        monkey->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 0)));
        monkey->add_uniform_data("u_color", glm::vec4(0.3f, 0.2f, 0.8f, 1.0f));

        cube->add_uniform_data("u_color", glm::vec4(0.2f, 0.8f, 0.2f, 1.0f));
        cube->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), glm::vec3(3, 0, 0)));
        
        checker_texture = Texture2D::create("./assets/textures/checkerboard.png");
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

        texture_shader->on_ui_render(true);
        camera->on_ui_render(true);
        enginegl::on_ui_render(true);
        //monkey->on_ui_render(true);
        gearbox->on_ui_render(true);
    }

    void on_update() override {
        float time = (float)glfwGetTime();
        float delta_time = time - last_frame_time;
        last_frame_time = time;
        {
            auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
            int state = glfwGetKey(window, GLFW_KEY_W);
            glm::vec3 camera_position = camera->get_position();
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
                
                camera_rotate = !camera_rotate;
                if (camera_rotate) {
                    GAME_INFO("Camera Rotating");
                } else {
                    GAME_INFO("Camera Stopped");
                }
            }
        }

        float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        if (camera_rotate) {
            camera->set_position(glm::vec3(camX, 0.0f, camZ));
        }
        square->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));

        Renderer::begin_scene(camera, glm::vec4{0.5f, 0.5f, 0.5f, 1.0f});

        checker_texture->bind();
        Renderer::submit_entity(texture_shader, square);
        Renderer::submit_entity(texture_shader, monkey);
        Renderer::submit_entity(texture_shader, cube);
        Renderer::submit_entity(color_shader, gearbox);
    }

private:
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> buffer; 
    std::shared_ptr<IndexBuffer> index_buffer;

    std::shared_ptr<Shader> color_shader;
    std::shared_ptr<Shader> texture_shader;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<Entity> square;
    std::shared_ptr<Entity> monkey;
    std::shared_ptr<Entity> cube;
    std::shared_ptr<Entity> gearbox;

    std::shared_ptr<Texture> checker_texture;

    glm::mat4 model_matrix {1.0f};
    glm::vec3 model_position {0.0f};
    
    float camera_move_speed = 5.0f;
    float model_move_speed = 2.0f;
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;

    bool camera_rotate = false;
};

int main() {
    engine::Application application{800, 800};
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
