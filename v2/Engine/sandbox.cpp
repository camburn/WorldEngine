#include "engine.hpp"
#include <iostream>
#include <memory>

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
        std::string vs_file_simple = "./shaders/opengl2_vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/opengl2_fragment_simple.glsl";

        std::string vs_file_color = "./shaders/opengl2_vertex.glsl";
        std::string fs_file_color = "./shaders/opengl2_fragment.glsl";

        std::string vs_file_texture = "./shaders/opengl2_vertex_texture.glsl";
        std::string fs_file_texture = "./shaders/opengl2_fragment_texture.glsl";

        texture_shader.reset(new Shader{ vs_file_texture, fs_file_texture });
        //color_shader.reset(new Shader{ vs_file_color, fs_file_color });
        simple_shader.reset(new Shader{ vs_file_simple, fs_file_simple });
        #else
        std::string vs_file_simple = "./shaders/vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/fragment_simple.glsl";

        std::string vs_file_color = "./shaders/vertex_color.glsl";
        std::string fs_file_color = "./shaders/fragment_color.glsl";

        std::string vs_file = "./shaders/vertex.glsl";
        std::string fs_file = "./shaders/fragment.glsl";

        texture_shader.reset(new Shader{ vs_file, fs_file });
        //color_shader.reset(new Shader{ vs_file_color, fs_file_color });
        simple_shader.reset(new Shader{ vs_file_simple, fs_file_simple });
        #endif

        //camera.reset(new OrthographicCamera {-2.0f, 2.0f, -2.0f, 2.0f} );
        
        width = Application::get().get_window().get_width();
        height = Application::get().get_window().get_height();
        ENGINE_INFO("Window: {0} x {1}", width, height);
        float aspect = (float)width / (float)height;

        camera.reset(new PerspectiveCamera { 45.0f, aspect, 0.1f, 100.0f });

        camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));

        // TODO: Connect mesh index information to the rendered::submit call
        // It requires object count and data type (uint/ushort)
        entities["helmet"] = GltfEntity::load_from_file(
            "./assets/gltf/DamagedHelmet/DamagedHelmet.gltf"
        );
        entities["flight_helmet"] = GltfEntity::load_from_file(
            "./assets/gltf/FlightHelmet/FlightHelmet.gltf"
        );
        cube = GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");
        //monkey = GltfEntity::load_from_file("./assets/gltf/Monkey/monkey.gltf");
        entities["monkey"] = GltfEntity::load_from_file("./assets/gltf/Monkey/monkey.gltf");
        square.reset( new CustomEntity());
        
        std::vector<glm::vec4> data = {
            {  5.0f, 0.0f,  5.0f, 1.0f },
            {  5.0f, 0.0f, -5.0f, 1.0f },
            { -5.0f, 0.0f, -5.0f, 1.0f },
            { -5.0f, 0.0f,  5.0f, 1.0f }
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

        entities["monkey"]->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 0)));

        entities["helmet"]->add_uniform_data("u_model", glm::mat4(1.0f));

        entities["flight_helmet"]->add_uniform_data("u_model",
            glm::translate(glm::mat4(1.0f), glm::vec3(3, 0, 0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(4, 4, 4))
        );
        //gearbox->add_uniform_data("u_model", glm::scale(glm::mat4(1.0f), glm::vec3(0.25, 0.25, 0.25)));
        
        //checker_texture = Texture2D::create("./assets/textures/checkerboard.png");
        dirt_albedo_texture = Texture2D::create(
            "./assets/textures/dry-dirt1-albedo_small.png"
        );
        dirt_normal_texture = Texture2D::create(
            "./assets/textures/dry-dirt1-normal_small.png"
        );
        dirt_rma_texture = Texture2D::create(
            "./assets/textures/dry-dirt1-rma.png"
        );

        for (auto& [name, entity]: entities) {
            entity->update_buffers(texture_shader);
        }
    }

    void on_attach() override {
        GAME_INFO("My layer attached");
    }

    void on_ui_render() override {
        ImGui::Begin("Entities");
        ImGui::InputInt("Render Mode", &render_mode);
        ImGui::Checkbox("Rotate Camera", &camera_rotate);
        
        ImGui::SetNextItemWidth(50.f);
        ImGui::InputFloat("Camera radius", &camera_radius);
        ImGui::InputFloat3("Dir Light Position", &light_position.x);
        ImGui::InputFloat3("Dir Light Color", &light_color.r);
        ImGui::Separator();
        ImGui::InputFloat("Light A radius", &light_radius);
        ImGui::Checkbox("Rotate Light A", &light_rotate);
        ImGui::InputFloat3("Point Light Position A", &light_position_b.x);
        ImGui::InputFloat3("Point Light Color A", &light_color_b.r);
        ImGui::Separator();
        ImGui::InputFloat3("Point Light Position B", &light_position_c.x);
        ImGui::InputFloat3("Point Light Color B", &light_color_c.r);
        glm::vec3 camera_pos = camera->get_position();
        ImGui::InputFloat3("Camera Position", &camera_pos.x);
        
        ImGui::Separator();
        ImGui::Text("Entities");
        for (auto& [name, entity]: entities) {
            if (ImGui::TreeNode(name.c_str())) {
                glm::vec3 translation = entity->get_translation();
                glm::vec3 scale = entity->get_scale();
                glm::quat rotation = entity->get_rotation();

                ImGui::InputFloat3("Translation", &translation.x);
                ImGui::InputFloat3("Scale", &scale.x);
                ImGui::InputFloat4("Rotation", &rotation.x);
                ImGui::Checkbox("Draw", &entity->draw);

                entity->set_translation(translation);
                entity->set_scale(scale);
                entity->set_rotation(rotation);

                ImGui::TreePop();
            }
        }
        ImGui::End();

        texture_shader->on_ui_render(true);
        camera->on_ui_render(true);
        enginegl::on_ui_render(true);
        //monkey->on_ui_render(true);
        entities["helmet"]->on_ui_render(true);
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

        if (camera_rotate) {
            float camX = sin(glfwGetTime()) * camera_radius;
            float camZ = cos(glfwGetTime()) * camera_radius;
            camera->set_position(glm::vec3(camX, 0.0f, camZ));
        }
        if (light_rotate) {
            float lightX = sin(glfwGetTime()) * light_radius;
            float lightZ = cos(glfwGetTime()) * light_radius;
            light_position_b = glm::vec3(lightX, 0.0f, lightZ);
        }
        square->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));

        texture_shader->bind();
        //texture_shader->upload_u_vec3("u_lightpos[0]",
        //    std::vector<glm::vec3> { light_position, light_position_b });
        //texture_shader->upload_u_vec3("u_lightcolor[0]",
        //    std::vector<glm::vec3> { light_color, light_color_b });
        texture_shader->upload_u_vec3("u_lightpos[0]", light_position);
        texture_shader->upload_u_vec3("u_lightpos[1]", light_position_b);
        texture_shader->upload_u_vec3("u_lightpos[2]", light_position_c);
        texture_shader->upload_u_vec3("u_lightcolor[0]", light_color);
        texture_shader->upload_u_vec3("u_lightcolor[1]", light_color_b);
        texture_shader->upload_u_vec3("u_lightcolor[2]", light_color_c);
        
        texture_shader->upload_u_vec3("u_camera_position", camera->get_position());
        texture_shader->upload_u_int1("u_render_mode", render_mode);

        //cube->add_uniform_data("u_color", glm::vec4(light_color, 1.0f));
        //cube->add_uniform_data("u_model", 
        //    glm::translate(glm::mat4(1.0f), light_position) *
        //    glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 0.1))
        //);
        width = Application::get().get_window().get_width();
        height = Application::get().get_window().get_height();
        if (width != Application::get().get_window().get_width()
            || height != Application::get().get_window().get_height()
        ) {
            width = Application::get().get_window().get_width();
            height = Application::get().get_window().get_height();
            float aspect = (float)width / (float)height;
            std::static_pointer_cast<PerspectiveCamera>(camera)->set_proj_matrix(45.0f, aspect, 0.1f, 100.0f);
        }
        Renderer::begin_scene(camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, width, height });

        dirt_albedo_texture->bind(0);
        dirt_normal_texture->bind(1);
        dirt_rma_texture->bind(2);
        dirt_rma_texture->bind(3);
        Renderer::submit_entity(texture_shader, square);
        
        for (auto& [name, entity]: entities) {
            if (entity->draw)
                Renderer::submit_entity(texture_shader, entity);
        }

        // Draw light positions
        simple_shader->bind();
        cube->add_uniform_data("u_color", glm::vec4(light_color_b, 1.0f));
        cube->add_uniform_data("u_model", 
            glm::translate(glm::mat4(1.0f), light_position_b) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.05, 0.05))
        );
        Renderer::submit_entity(simple_shader, cube);
        
        cube->add_uniform_data("u_color", glm::vec4(light_color_c, 1.0f));
        cube->add_uniform_data("u_model", 
            glm::translate(glm::mat4(1.0f), light_position_c) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.05, 0.05))
        );
        Renderer::submit_entity(simple_shader, cube);
    }

private:
    //std::shared_ptr<Shader> color_shader;
    std::shared_ptr<Shader> texture_shader;
    std::shared_ptr<Shader> simple_shader;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<Entity> square;
    std::shared_ptr<Entity> cube;

    std::map<std::string, std::shared_ptr<Entity>> entities;

    std::shared_ptr<Texture> checker_texture;
    std::shared_ptr<Texture> dirt_rma_texture;
    std::shared_ptr<Texture> dirt_albedo_texture;
    std::shared_ptr<Texture> dirt_normal_texture;

    glm::mat4 model_matrix {1.0f};
    glm::vec3 model_position {0.0f, -5.0f, 0.0f};
    glm::vec3 light_position {5, 5, 5};
    glm::vec3 light_color {150, 150, 150};
    glm::vec3 light_position_b {1, 0, 0};
    glm::vec3 light_color_b {150, 0, 0};
    glm::vec3 light_position_c {-1, 0, 0};
    glm::vec3 light_color_c {0, 150, 0};
    
    float camera_move_speed = 5.0f;
    float model_move_speed = 2.0f;
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;

    float camera_radius = 1.0f;
    float light_radius = 1.0f;
    bool camera_rotate = false;
    bool light_rotate = false;
    bool draw_gears = true;

    int render_mode = 0;

    int width = 1200;
    int height = 800;
};

int main() {
    engine::Application application{1200, 800};
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
