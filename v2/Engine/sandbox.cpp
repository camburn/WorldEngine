#include "engine.hpp"
#include <iostream>
#include <memory>
#include <tuple>

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

std::unordered_map<int, std::string> render_modes {
    {0, "Complete"},
    {1, "Albedo Map"},
    {2, "Metallic Map"},
    {3, "Roughness Map"},
    {4, "Ambient Occlusion Map"},
    {5, "Emission Map"},
    {6, "Lo"},
    {7, "Lighting"},
    {8, "Fresnel"},
    {9, "Irradiance"},
    {10, "Reflection"},
    {11, "Face Normals"},
    {12, "Normal Map"},
    {13, "Texture Coordinates"},
    {14, "Transparency"},
    {15, "Shadows"},
};

class MyLayer: public engine::Layer {
public:
    MyLayer() {

        #ifdef OPENGL_COMPATIBILITY
        std::string vs_file_simple = "./shaders/opengl2_vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/opengl2_fragment_simple.glsl";

        std::string vs_file_texture = "./shaders/opengl2_vertex_texture.glsl";
        std::string fs_file_texture = "./shaders/opengl2_fragment_texture.glsl";

        std::string vs_file_ibl_equi_to_cube = "./shaders/IBL/vertex_equi_to_cubemap.glsl";
        std::string fs_file_ibl_equi_to_cube = "./shaders/IBL/fragment_equi_to_cubemap.glsl";

        std::string vs_skybox = "./shaders/IBL/vertex_skybox.glsl";
        std::string fs_skybox = "./shaders/IBL/fragment_skybox.glsl";

        std::string vs_shadow_mapper = "./shaders/shadow_mapping/vertex_depth_map.glsl";
        std::string fs_shadow_mapper = "./shaders/shadow_mapping/fragment_depth_map.glsl";

        texture_shader.reset(new Shader{ vs_file_texture, fs_file_texture });
        depth_map_shader.reset(new Shader{ vs_shadow_mapper, fs_shadow_mapper });
        simple_shader.reset(new Shader{ vs_file_simple, fs_file_simple });
        ibl_equi_to_cube_shader.reset(new Shader{ vs_file_ibl_equi_to_cube, fs_file_ibl_equi_to_cube });
        skybox.reset(new Shader{ vs_skybox, fs_skybox });

        #else
        std::string vs_file_simple = "./shaders/vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/fragment_simple.glsl";

        std::string vs_file_texture = "./shaders/vertex.glsl";
        std::string fs_file_texture = "./shaders/fragment.glsl";

        std::string vs_file_ibl_equi_to_cube = "./shaders/IBL/vertex_equi_to_cubemap3.glsl";
        std::string fs_file_ibl_equi_to_cube = "./shaders/IBL/fragment_equi_to_cubemap3.glsl";

        std::string vs_skybox = "./shaders/IBL/vertex_skybox3.glsl";
        std::string fs_skybox = "./shaders/IBL/fragment_skybox3.glsl";

        std::string vs_shadow_mapper = "./shaders/shadow_mapping/vertex_depth_map3.glsl";
        std::string fs_shadow_mapper = "./shaders/shadow_mapping/fragment_depth_map3.glsl";

        texture_shader.reset(new Shader{ vs_file_texture, fs_file_texture });
        depth_map_shader.reset(new Shader{ vs_shadow_mapper, fs_shadow_mapper });
        simple_shader.reset(new Shader{ vs_file_simple, fs_file_simple });
        ibl_equi_to_cube_shader.reset(new Shader{ vs_file_ibl_equi_to_cube, fs_file_ibl_equi_to_cube });
        skybox.reset(new Shader{ vs_skybox, fs_skybox });
        #endif

        // --- IBL CALCULATION -- 
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f, // 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f, // 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, //-1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, //-1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, //-1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, //-1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, //-1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, //-1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f, // 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f, // 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f, // 1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f, // 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f, // 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f, // 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f, // 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f, // 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f, // 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f, // 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, // 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f, // 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f, // 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f, // 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f, // 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f, // 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f, // 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f, // 0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };

        cube_vao = VertexArray::create();
        auto cube_vbo = VertexBuffer::create(vertices, sizeof(vertices));
        cube_vbo->set_layout({
            { engine::ShaderDataType::Float3, "position" },
        });
        cube_vao->add_vertex_buffer(cube_vbo);
        cube_vao->set_array_count(36);

        std::shared_ptr<PerspectiveCamera> ibl_camera( new PerspectiveCamera(90.0f, 1.0f, 0.1, 10.0f));
        struct camera_position {
            glm::vec3 position;
            glm::vec3 look_at;
            glm::vec3 up;
        };
        std::vector<camera_position> views = {
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)},
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)},
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
            {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)}
        };
        
        ibl_equi_to_cube_shader->bind();
        
        hdr_map = TextureHDR::create(
            "./assets/hdr/Arches_E_PineTree_3k.hdr"
        );
        
        environment_map = TextureCubeMap::create(512, 512);
        
        auto fbo = FrameBuffer::create(512, 512);

        hdr_map->bind(0);
        fbo->bind();
        for (unsigned int i = 0; i < 6; i++) {
            // Why use a tuple over a struct?
            auto view = views.at(i);
            ibl_camera->set_view(view.position, view.look_at, view.up);
            Renderer::begin_scene(ibl_camera, { glm::vec4(1.0f), 512, 512 });
            environment_map->set_data(i);
            
            Renderer::submit(ibl_equi_to_cube_shader, cube_vao, glm::mat4(1.0f));
        }
        fbo->unbind();
        
        ibl_equi_to_cube_shader->unbind();
        ibl_camera.reset();
        
        // --- END IBL ---

        width = Application::get().get_window().get_width();
        height = Application::get().get_window().get_height();
        ENGINE_INFO("Window: {0} x {1}", width, height);
        float aspect = (float)width / (float)height;

        //camera.reset(new OrthographicCamera {-2.0f, 2.0f, -2.0f, 2.0f} );
        shadow_camera.reset( new OrthographicCamera {-5.0f, 5.0f, -5.0f, 5.0f} );
        camera.reset(new PerspectiveCamera { 75.0f, aspect, 0.1f, 100.0f });

        camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));

        // TODO: Connect mesh index information to the rendered::submit call
        // It requires object count and data type (uint/ushort)
        entities["helmet"] = GltfEntity::load_from_file(
            "./assets/gltf/DamagedHelmet/DamagedHelmet.gltf"
        );
        entities["helmet"]->name = "Damaged Helmet";
        entities["flight_helmet"] = GltfEntity::load_from_file(
            "./assets/gltf/FlightHelmet/FlightHelmet.gltf"
        );
        entities["flight_helmet"]->name = "Flight Helmet";

        cube = GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");

        entities["cube"] = GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");
        entities["cube"]->name = "Cube";
        //monkey = GltfEntity::load_from_file("./assets/gltf/Monkey/monkey.gltf");
        entities["monkey"] = GltfEntity::load_from_file("./assets/gltf/Monkey/monkey.gltf");
        entities["monkey"]->name = "Monkey";
        square.reset( new CustomEntity());
        square->name = "Square";
        
        std::vector<glm::vec4> data = {
            {  3.0f, 0.0f,  3.0f, 1.0f },
            {  3.0f, 0.0f, -3.0f, 1.0f },
            { -3.0f, 0.0f, -3.0f, 1.0f },
            { -3.0f, 0.0f,  3.0f, 1.0f }
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

        entities["cube"]->add_uniform_data("u_color", glm::vec4(1.0f));
        entities["cube"]->add_uniform_data("u_model", 
            glm::translate(glm::mat4(1.0f), glm::vec3(1, -1.5, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5))
        );
        
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

        // SHADOW MAP SETUP
        shadow_map = TextureDepth::create(shadow_map_width, shadow_map_height);
        shadow_map_buffer = FrameBuffer::create(shadow_map);
    }

    void on_attach() override {
        GAME_INFO("My layer attached");
    }

    void on_ui_render() override {
        ImGui::Begin("Entities");
        ImGui::Text("Render Mode: %s", render_modes[render_mode].c_str());
        ImGui::InputInt("Change Mode", &render_mode);
        
        ImGui::Checkbox("Rotate Camera", &camera_rotate);
        ImGui::Checkbox("Shadow Camera", &use_shadow_cam);
        
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
        // ===== CONTROLS =====
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
            light_position = glm::vec3(lightX, light_position.y, lightZ);
        }
        square->add_uniform_data("u_model", glm::translate(glm::mat4(1.0f), model_position));
        // === END CONTROLS ===

        // ===== SHADOW MAP =====
        shadow_camera->set_position(light_position);
        
        shadow_map->bind();
        // Bind Shadow map shader here?!
        depth_map_shader->bind();
        shadow_map_buffer->bind();
        // Render things here
        Renderer::begin_scene(shadow_camera, { glm::vec4(1.0f), shadow_map_width, shadow_map_height });
        Renderer::submit_entity(depth_map_shader, square);
        
        for (auto& [name, entity]: entities) {
            if (entity->draw) {
                Renderer::submit_entity(depth_map_shader, entity);
            }
        }
        depth_map_shader->unbind();
        shadow_map_buffer->unbind();
        // === END SHADOW MAP ===

        texture_shader->bind();
        texture_shader->upload_u_vec3("u_lightpos[0]", light_position);
        texture_shader->upload_u_vec3("u_lightpos[1]", light_position_b);
        texture_shader->upload_u_vec3("u_lightpos[2]", light_position_c);
        texture_shader->upload_u_vec3("u_lightcolor[0]", light_color);
        texture_shader->upload_u_vec3("u_lightcolor[1]", light_color_b);
        texture_shader->upload_u_vec3("u_lightcolor[2]", light_color_c);
        
        texture_shader->upload_u_vec3("u_camera_position", camera->get_position());
        texture_shader->upload_u_int1("u_render_mode", render_mode);

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
        if (use_shadow_cam) {
            Renderer::begin_scene(shadow_camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, shadow_map_width, shadow_map_height });
        } else {
            Renderer::begin_scene(camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, width, height });
        }
        texture_shader->upload_u_mat4("u_light_space_matrix", shadow_camera->get_view_projection_matrix());
        shadow_map->bind(texture_shader->uniform_texture_unit("shadow_map"));

        dirt_albedo_texture->bind(0);
        dirt_normal_texture->bind(3);
        dirt_rma_texture->bind(4);
        dirt_rma_texture->bind(1);
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

        skybox->bind();
        environment_map->bind(0);
        skybox->upload_u_mat4("u_view", camera->get_view_matrix());
        skybox->upload_u_mat4("u_projection", camera->get_projection_matrix());
        Renderer::submit(skybox, cube_vao, glm::mat4(1.0f));

    }

private:
    std::shared_ptr<Shader> ibl_equi_to_cube_shader;
    std::shared_ptr<Shader> skybox;

    std::shared_ptr<Shader> texture_shader;
    std::shared_ptr<Shader> simple_shader;
    std::shared_ptr<Shader> depth_map_shader;

    std::shared_ptr<Camera> camera;
    std::shared_ptr<OrthographicCamera> shadow_camera;

    std::shared_ptr<VertexArray> cube_vao;

    std::shared_ptr<Entity> square;
    std::shared_ptr<Entity> cube;

    std::map<std::string, std::shared_ptr<Entity>> entities;

    std::shared_ptr<Texture> checker_texture;
    std::shared_ptr<Texture> dirt_rma_texture;
    std::shared_ptr<Texture> dirt_albedo_texture;
    std::shared_ptr<Texture> dirt_normal_texture;
    std::shared_ptr<Texture> hdr_map;

    std::shared_ptr<TextureDepth> shadow_map;
    std::shared_ptr<FrameBuffer> shadow_map_buffer;

    std::shared_ptr<TextureCubeMap> environment_map;

    glm::mat4 model_matrix {1.0f};
    glm::vec3 model_position {0.0f, -2.0f, 0.0f};
    glm::vec3 light_position {3, 3, 3};
    glm::vec3 light_color {750, 750, 750};
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
    bool use_shadow_cam = false;

    int render_mode = 0;

    int width = 1200;
    int height = 800;

    int shadow_map_width = 1024;
    int shadow_map_height = 1024;
};

int main() {
    engine::Application application{1200, 800};
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
