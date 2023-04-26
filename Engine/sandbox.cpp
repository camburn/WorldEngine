#include "engine.hpp"
#include <iostream>
#include <memory>
#include <tuple>
#include <limits>

#include "imgui.h"
#include "Tools/ImGuizmo.h"

#include "Engine/application.hpp"
#include "Engine/layer.hpp"
#include "Engine/objects.hpp"
#include "Engine/manager.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"
#include "Engine/renderer/renderer.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/lights.hpp"
#include "Engine/renderer/debug_draw.hpp"
#include "Engine/entity.hpp"
#include "Engine/scripts.hpp"
#include "Engine/python_api.hpp"
#include "Tools/gltf_loader.hpp"
#include "Engine/physics/raycast.hpp"
#include "Engine/physics/collision.hpp"
#include "Engine/renderer/texture.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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
    {10, "Calculated Normal"},
    {11, "Face Normals"},
    {12, "Normal Map"},
    {13, "Texture Coordinates"},
    {14, "Transparency"},
    {15, "Direction Shadow Map"},
    {16, "Point Shadow Map"},
    {17, "Reflection"},
    {18, "Specular"},
    {19, "Ambient"},
    {20, "Diffuse"},
    {21, "kD"},
};


const glm::vec4 RED = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
const glm::vec4 GREEN = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
const glm::vec4 BLUE = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
const glm::vec4 WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 BLACK = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


class MyLayer: public engine::Layer {
public:

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

    struct lookat_up {
            glm::vec3 look_at;
            glm::vec3 up;
    };

    std::vector<lookat_up> point_shadow_views = {
        //target, up
        {glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
        {glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
        {glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)},
        {glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)},
        {glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)},
        {glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)}
    };

    MyLayer() {

        #ifdef OPENGL_COMPATIBILITY
        std::string vs_file_simple = "./shaders/opengl2_vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/opengl2_fragment_simple.glsl";

        std::string vs_file_texture = "./shaders/opengl2_vertex_texture.glsl";
        std::string fs_file_texture = "./shaders/opengl2_fragment_texture.frag";

        std::string vs_shadow_mapper = "./shaders/shadow_mapping/vertex_depth_map.glsl";
        std::string fs_shadow_mapper = "./shaders/shadow_mapping/fragment_depth_map.glsl";

        // IBL Shaders
        std::string vs_file_ibl_equi_to_cube = "./shaders/IBL/vertex_cubemap.glsl";
        std::string fs_file_ibl_equi_to_cube = "./shaders/IBL/fragment_equi_to_cubemap.glsl";

        std::string vs_file_convolution_map = "./shaders/IBL/vertex_cubemap.glsl";
        std::string fs_file_convolution_map = "./shaders/IBL/fragment_convolution_cubemap.glsl";

        std::string vs_file_prefilter = "./shaders/IBL/vertex_cubemap.glsl";
        std::string fs_file_prefilter = "./shaders/IBL/fragment_prefilter.glsl";

        std::string vs_file_brdf = "./shaders/IBL/vertex_brdf.glsl";
        std::string fs_file_brdf = "./shaders/IBL/fragment_brdf.glsl";

        std::string vs_skybox = "./shaders/IBL/vertex_skybox.glsl";
        std::string fs_skybox = "./shaders/IBL/fragment_skybox.glsl";

        #else
        std::string vs_file_simple = "./shaders/vertex_simple.glsl";
        std::string fs_file_simple = "./shaders/fragment_simple.glsl";

        std::string vs_file_texture = "./shaders/vertex.glsl";
        std::string fs_file_texture = "./shaders/fragment.glsl";

        std::string vs_shadow_mapper = "./shaders/shadow_mapping/vertex_depth_map3.glsl";
        std::string fs_shadow_mapper = "./shaders/shadow_mapping/fragment_depth_map3.glsl";

        // IBL Shaders
        std::string vs_file_ibl_equi_to_cube = "./shaders/IBL/vertex_cubemap3.glsl";
        std::string fs_file_ibl_equi_to_cube = "./shaders/IBL/fragment_equi_to_cubemap3.glsl";

        std::string vs_file_convolution_map = "./shaders/IBL/vertex_cubemap3.glsl";
        std::string fs_file_convolution_map = "./shaders/IBL/fragment_convolution_cubemap3.glsl";

        std::string vs_file_prefilter = "./shaders/IBL/vertex_cubemap3.glsl";
        std::string fs_file_prefilter = "./shaders/IBL/fragment_prefilter3.glsl";

        std::string vs_file_brdf = "./shaders/IBL/vertex_brdf3.glsl";
        std::string fs_file_brdf = "./shaders/IBL/fragment_brdf3.glsl";

        std::string vs_skybox = "./shaders/IBL/vertex_skybox3.glsl";
        std::string fs_skybox = "./shaders/IBL/fragment_skybox3.glsl";
        #endif

        script_init(); // Initialise Python Scripting

        texture_shader.reset(new Shader{ vs_file_texture, fs_file_texture });
        depth_map_shader.reset(new Shader{ vs_shadow_mapper, fs_shadow_mapper });
        //simple_shader.reset(new Shader{ vs_file_simple, fs_file_simple });

        // IBL Shaders
        ibl_equi_to_cube_shader.reset(new Shader{ vs_file_ibl_equi_to_cube, fs_file_ibl_equi_to_cube });
        convolution_shader.reset(new Shader{ vs_file_convolution_map, fs_file_convolution_map });
        prefilter_shader.reset(new Shader{ vs_file_prefilter, fs_file_prefilter });
        brdf_shader.reset(new Shader{ vs_file_brdf, fs_file_brdf });
        skybox_shader.reset(new Shader{ vs_skybox, fs_skybox });

        engine_debug::init();
        // --- IBL CALCULATION --

        float vertices[] = {
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f
        };

        uint32_t indices[] = {
            0, 1, 3, 3, 1, 2,
            1, 5, 2, 2, 5, 6,
            5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3,
            3, 2, 7, 7, 2, 6,
            4, 5, 0, 0, 5, 1
        };

        cube_vao = VertexArray::create();
        auto cube_vbo = VertexBuffer::create(vertices, sizeof(vertices));
        auto cube_ibo = IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
        cube_vbo->set_layout({
            { engine::ShaderDataType::Float3, "position" },
        });
        cube_vao->add_vertex_buffer(cube_vbo);
        cube_vao->set_index_buffer(cube_ibo);

        ibl_camera.reset(new PerspectiveCamera(90.0f, 1.0f, 0.1f, 10.0f));

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

        // Equirectangular to Cubemap
        ibl_equi_to_cube_shader->bind();

        hdr_map = TextureHDR::create("./assets/hdr/Arches_E_PineTree_3k.hdr");

        auto fbo = FrameBuffer::create(512, 512);
        fbo->bind();

        environment_map = TextureCubeMap::create(512, 512, RGB, false, NEAREST, LINEAR);
        hdr_map->bind(0);
        for (unsigned int i = 0; i < 6; i++) {
            // Why use a tuple over a struct?
            auto view = views.at(i);
            std::static_pointer_cast<PerspectiveCamera>(ibl_camera)->set_view(view.position, view.look_at, view.up);
            environment_map->set_data(i);
            Renderer::begin_scene(ibl_camera, { glm::vec4(0.0f), 512, 512 });
            Renderer::submit(ibl_equi_to_cube_shader, cube_vao, glm::mat4(1.0f));
        }
        fbo->unbind();
        hdr_map->unbind();
        ibl_equi_to_cube_shader->unbind();

        // Generate Mipmaps for environment map
        std::static_pointer_cast<TextureCubeMap>(environment_map)->generate_mipmaps();

        // Cubemap Convolution
        convolution_shader->bind();

        auto fbo_small = FrameBuffer::create(32, 32);
        fbo_small->bind();
        irradiance_map = TextureCubeMap::create(32, 32);

        environment_map->bind(0);
        for (unsigned int i = 0; i < 6; i++) {
            // Why use a tuple over a struct?
            auto view = views.at(i);
            std::static_pointer_cast<PerspectiveCamera>(ibl_camera)->set_view(view.position, view.look_at, view.up);
            irradiance_map->set_data(i);
            Renderer::begin_scene(ibl_camera, { glm::vec4(0.0f), 32, 32 });
            Renderer::submit(convolution_shader, cube_vao, glm::mat4(1.0f));
        }

        fbo_small->unbind();
        convolution_shader->unbind();
        environment_map->unbind();

        // pre-filter

        prefilter_map = TextureCubeMap::create(128, 128, RGB, true, LINEAR_MIPMAP_LINEAR, LINEAR);
        prefilter_map->bind();
        prefilter_shader->bind();
        fbo_filter = FrameBuffer::create(128, 128);

        environment_map->bind(0);
        fbo_filter->bind();

        unsigned int max_mip_levels = 5;
        for (unsigned int mip = 0; mip < max_mip_levels; ++mip) {
            unsigned int mip_width = (unsigned int)(128.0f * std::pow(0.5f, mip));
            unsigned int mip_height = (unsigned int)(128.0f * std::pow(0.5f, mip));
            std::static_pointer_cast<FrameBuffer>(fbo_filter)->resize(mip_width, mip_height);

            float roughness = (float)mip / (float)(max_mip_levels - 1);
            prefilter_shader->upload_u_float1("roughness", roughness);
            ENGINE_INFO("Pre Filter roughness: {0} - {1}x{2}", roughness, mip_width, mip_height);
            for (unsigned int i = 0; i < 6; i++) {
                auto view = views.at(i);
                std::static_pointer_cast<PerspectiveCamera>(ibl_camera)->set_view(view.position, view.look_at, view.up);
                prefilter_map->set_data(i, mip);
                Renderer::begin_scene(ibl_camera, { glm::vec4(0.0f), (int)mip_width, (int)mip_height });
                Renderer::submit(prefilter_shader, cube_vao, glm::mat4(1.0f));
            }
        }

        fbo_filter->unbind();
        environment_map->unbind();
        prefilter_shader->unbind();

        // Generate 2D LUT from BRDF equations
        // TODO
        std::vector<glm::vec2> quad_texcoords {
            // positions        // texture Coords
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f}
        };
        std::vector<glm::vec4> quad_vertices {
            // positions        // texture Coords
            {-1.0f,  1.0f, 0.0f, 1.0f},
            {-1.0f, -1.0f, 0.0f, 1.0f},
            { 1.0f,  1.0f, 0.0f, 1.0f},
            { 1.0f, -1.0f, 0.0f, 1.0f},
        };

        std::shared_ptr<Entity> quad( new CustomEntity(engine::DrawMode::TRIANGLE_STRIP));

        std::static_pointer_cast<CustomEntity>(quad)->add_attribute_data("position", quad_vertices);
        std::static_pointer_cast<CustomEntity>(quad)->add_attribute_data("texcoord", quad_texcoords);
        std::static_pointer_cast<CustomEntity>(quad)->name = "Quad";

        fbo_filter->bind();
        brdf_map = Texture2D::create(512, 512);
        //brdf_map->bind(0);

        std::static_pointer_cast<FrameBuffer>(fbo_filter)->resize(512, 512);
        std::static_pointer_cast<Texture2D>(brdf_map)->set_data();

        brdf_shader->bind();
        Renderer::begin_scene(ibl_camera, { glm::vec4(0.0f), (int)512, (int)512 });
        Transform t;
        Renderer::submit_entity(brdf_shader, quad, t);

        fbo_filter->unbind();

        brdf_map_file = Texture2D::create(
            "./assets/textures/BRDF_LUT.tga"
        );

        // --- END IBL ---

        width = Application::get().get_window().get_width();
        height = Application::get().get_window().get_height();
        ENGINE_INFO("Window: {0} x {1}", width, height);
        float aspect = (float)width / (float)height;

        shadow_camera.reset( new OrthographicCamera {-5.0f, 5.0f, -5.0f, 5.0f} );

        shadow_point_camera.reset(new PerspectiveCamera { 90.0f, (float)shadow_map_width / (float)shadow_map_height, 0.1f, 50.0f });

        debug_camera.reset(new NewPerspectiveCamera { 75.0f, aspect, 0.1f, 100.0f });
        camera.reset(new NewPerspectiveCamera { 45.0f, aspect, 0.1f, 25.0f });
        set_main_camera(camera);

        camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
        debug_camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));

        texture_shader->bind();

        deserialise_assets();
        deserialise_object();

        for (auto& [name, entity]: m_entities) {
            entity->update_buffers(texture_shader);
            entity->draw = false;
        }

        for (auto& [name, object]: m_objects) {
            if (!object->attached(Object::COLLIDER)
            && object->attached(Object::MESH) ) {
                // This size calculation is a bit trickey, it cannot occur until
                // the mesh data is set by calling update_buffers.
                glm::vec3 size = object->mesh()->max_extents - object->mesh()->min_extents;
                glm::vec4 m_size = glm::vec4(size, 1.0f) * object->transform().get_model_matrix();
                object->attach(
                    std::shared_ptr<BoxCollider>(
                        new BoxCollider{
                            glm::vec3(0.0f),
                            glm::vec3(m_size)
                        }
                    )
                );
                object->collider()->transform.set_parent(object->transform_reference());
            }
        }

        sun_directional_light = m_objects["sky_light"];
        point_light_green = m_objects["green_light"];

        // SHADOW MAP SETUP
        shadow_map = TextureDepth::create(shadow_map_width, shadow_map_height);
        shadow_map_buffer = FrameBuffer::create(shadow_map);

        shadow_point_map = TextureCubeMap::create(shadow_map_width, shadow_map_height, engine::DEPTH_COMPONENT, false, NEAREST, NEAREST);
        shadow_point_map_buffer = FrameBuffer::create(shadow_point_map);

    }

    void on_attach() override {
        GAME_INFO("My layer attached");
    }

    void on_ui_render() override {
        ImGuizmo::BeginFrame();
        static bool p_open = true;

        static bool opt_fullscreen_persistant = true;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        bool opt_fullscreen = opt_fullscreen_persistant;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 test = {viewport->Size.x, viewport->Size.y};
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(test);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
        }

        if (show_scene_window) {
            ImGui::Begin("Scene Settings", &show_scene_window);
            ImGui::Text("Render Mode: %s", render_modes[render_mode].c_str());
            ImGui::InputInt("Change Mode", &render_mode);
            ImGui::InputInt("Skybox Mode", &skybox_mode);
            ImGui::Checkbox("Render Skybox", &render_skybox);
            ImGui::Checkbox("Cast Shadows", &cast_shadows);
            ImGui::Checkbox("Generated BRDF LUT", &use_generated_brdf);

            ImGui::Checkbox("Rotate Camera", &camera_rotate);
            ImGui::Checkbox("Shadow Camera", &use_shadow_cam);
            ImGui::Checkbox("Debug Camera", &use_debug_cam);

            ImGui::SetNextItemWidth(50.f);

            ImGui::InputFloat("Camera radius", &camera_radius);
            glm::vec3 camera_pos = camera->get_position();
            ImGui::InputFloat3("Camera Position", &camera_pos.x);

            int misc_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoInputs;
            ImGui::Text("Point Light A");
            ImGui::InputFloat("Light A radius", &light_radius);
            ImGui::Checkbox("Rotate Light A", &light_rotate);

            glm::vec3 shadow_cam_pos = sun_directional_light->transform().get_translation();
            ImGui::InputFloat3("Shadow Camera Position", &shadow_cam_pos.x);


            ImGui::End(); // Scene Settings
        }

        if (show_entity_window) {
            ImGui::Begin("Scene Browser", &show_entity_window);

            // New selectable menu
            static int selected = 0;
            static std::string selected_name = "";

            ImGui::Text("Name");
            ImGui::SameLine(ImGui::GetWindowWidth()-100);
            ImGui::Text("Enabled");

            ImGui::BeginChild("left pane", ImVec2(0, 0), true);

            int index = 0;
            for (auto& [name, object]: m_objects) {
                if (ImGui::Selectable(object->name.c_str(), selected == index, 0, ImVec2(ImGui::GetWindowWidth()-100, 18))) {
                    selected = index;
                    selected_name = name;
                }
                ImGui::SameLine(ImGui::GetWindowWidth()-50);
                
                if (object->attached(Object::MESH))
                    ImGui::Checkbox(("##" + name).c_str(), &object->mesh()->draw);
                if (object->attached(Object::LIGHT))
                    ImGui::Checkbox(("##" + name).c_str(), &object->light().enabled);
                index ++;
            }
            ImGui::EndChild();
            ImGui::End();
            //ImGui::SameLine();

            //ImGui::BeginGroup();
            //ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Begin("Entity Properties");
            if (m_objects.count(selected_name) > 0) {
                auto &object = m_objects.at(selected_name);

                // Add control widget
                ImGui::Text("MyObject: %s", object->name.c_str());
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Object")){
                        ImGui::Text("%s", object->name.c_str());

                        /*
                        glm::mat4 eye = glm::mat4{1.0f};
                        ImGuizmo::DrawGrid(
                            &camera->get_view_matrix()[0][0],
                            &camera->get_projection_matrix()[0][0],
                            &eye[0][0],
                            10.f
                        );
                        */

                        glm::mat4 model_mat = object->transform().get_model_matrix();
                        EditTransform(std::static_pointer_cast<Camera>(current_camera), model_mat);
                        object->transform().set_model_matrix(model_mat);

                        ImGui::EndTabItem();
                    }
                    if (object->attached(Object::MESH) && ImGui::BeginTabItem("Mesh"))
                    {
                        glm::vec3 translation = object->transform().get_translation();
                        glm::vec3 scale = object->transform().get_scale();
                        glm::quat rotation = object->transform().get_rotation();

                        ImGui::Checkbox("Draw", &object->mesh()->draw);

                        ImGui::InputFloat3("Translation", &translation.x);
                        ImGui::InputFloat3("Scale", &scale.x);
                        ImGui::InputFloat4("Rotation", &rotation.x);
                        object->transform().set_translation(translation);
                        object->transform().set_scale(scale);
                        object->transform().set_rotation(rotation);

                        ImGui::EndTabItem();
                    }
                    if (object->attached(Object::LIGHT) && ImGui::BeginTabItem("Light")) {
                        int misc_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoInputs;
                        ImGui::Text("%s", object->name.c_str());
                        ImGui::Checkbox("Cast Shadows", &object->light().cast_shadows);
                        ImGui::Checkbox("Enabled", &object->light().enabled);
                        ImGui::Checkbox("Direction", &object->light().direction);
                        ImGui::ColorEdit3("Color", &object->light().color.r, misc_flags);
                        glm::vec3 hdr_color = object->light().get_hdr_color();
                        ImGui::InputFloat3("HDR Value", &hdr_color.r);
                        ImGui::InputFloat3("Position", &object->light().position.x);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Script")){
                        if (object->attached(Object::SCRIPT)) {
                            ImGui::Text("%s", object->script()->name.c_str());
                            if (ImGui::Button("Save")) {
                                object->script()->reload();
                            }
                            ImGui::InputTextMultiline(
                                "##Code",
                                &object->script()->source[0],
                                object->script()->source.size(),
                                ImVec2(-FLT_MIN, -FLT_MIN)
                            );
                        } else {
                            ImGui::Text("No Script");
                            if (ImGui::Button("Create")) {
                                object->attach(
                                    (std::shared_ptr<Script>)new PythonScript(
                                        object->name + "_script", object
                                    )
                                );
                            }
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Collider")){
                        
                        if (object->attached(Object::COLLIDER)) {
                            object->collider()->debug_draw_enabled = true;
                            ImGui::Text("Collider enabled");
                            ImGui::InputFloat3("Size", &std::static_pointer_cast<BoxCollider>(object->collider())->size.x);
                            glm::vec3 pos = object->collider()->transform.get_translation();
                            ImGui::InputFloat3("Position", &pos.x);
                            

                        }
                        ImGui::EndTabItem();
                    } else {
                        if (object->attached(Object::COLLIDER)) {
                            object->collider()->debug_draw_enabled = false;
                            glm::vec3 pos = object->collider()->transform.get_translation(true);
                            ImGui::InputFloat3("Position", &pos.x, ImGuiInputTextFlags_ReadOnly);
                        }
                    }
                    ImGui::EndTabBar();
                }
            }
            //ImGui::EndChild();
            //ImGui::EndGroup();

            ImGui::End();
        }

        if (ImGui::BeginMenuBar())  // Main Dockspace menu
        {
            if (ImGui::BeginMenu("Docking")) {
                if (ImGui::MenuItem("Flag: Test", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                ImGui::Separator();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Entities", NULL, &show_entity_window);
                ImGui::MenuItem("Scene Settings", NULL, &show_scene_window);

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End(); // Dockspace Demo

        ImGuizmo::Enable(true);

        texture_shader->on_ui_render(true);
        camera->on_ui_render(true);
        enginegl::on_ui_render(true);
    }

    void on_update() override {
        float time = (float)glfwGetTime();
        float delta_time = time - last_frame_time;
        last_frame_time = time;

        // ===== SCRIPTING =====
        for (auto &[name, object]: m_objects) {
            if (object->script() != nullptr) {
                object->script()->update(delta_time);
            }
        }
        // === END SCRIPTING ===
        engine_debug::new_frame();
        // ===== CONTROLS =====
        // Check if mouse is over UI elements, otherwise pass to camera
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            if (use_debug_cam) {
                std::static_pointer_cast<NewPerspectiveCamera>(debug_camera)->update(delta_time);
            } else {
                camera->update(delta_time);
            }
        }
        // RAYCAST SAMPLE
        
        static bool ray_set = false;
        //static RayHit hit;
        static Ray ray;
        
        if (!io.WantCaptureMouse && ImGui::IsMouseClicked(0) ){
            ENGINE_INFO("Casting ray");
            auto cam = std::static_pointer_cast<Camera>(camera);
            ray = cast_ray(cam);
            ray_set = true;
        }
        engine_debug::draw_simple_line(ray.origin, ray.project(50.0f), RED);
        if (ray_set) {
            for (auto &[name, object]: m_objects) {
                if (object->collider() != nullptr) {
                    RayHit hit = object->collider()->intersect(ray);
                    if (hit) {
                        bus::publish(std::make_unique<RaycastHitEvent>(ray, hit));
                        ENGINE_WARN("Raycast hit on collider");
                        object->collider()->debug_color = glm::vec4(1.0f, 0.3f, 0.3f, 1.0f);
                        ray_set = false;
                        break;
                    } else {
                        object->collider()->debug_color = glm::vec4(0.3f, 1.0f, 0.3f, 1.0f);
                    }
                }
            }
        }
        
        // END RAYCAST

        if (camera_rotate) {
            float camX = sin((float)glfwGetTime()) * camera_radius;
            float camZ = cos((float)glfwGetTime()) * camera_radius;
            camera->set_position(glm::vec3(camX, 0.0f, camZ));
        }
        if (light_rotate) {
            float lightX = (float)sin(glfwGetTime()) * light_radius;
            float lightZ = (float)cos(glfwGetTime()) * light_radius;
            sun_directional_light->transform().set_translation(glm::vec3(lightX, sun_directional_light->transform().get_translation().y, lightZ));
        }

        // === END CONTROLS ===

        // ===== SHADOW MAP =====
        float fov = camera->fov;
        float ar = camera->aspect;
        float near_plane = camera->near_plane;
        float far_plane = camera->far_plane;

        glm::vec3 cam_center = camera->get_position() + (
            camera->get_forward_direction() * (near_plane + far_plane / 2)
        );
        shadow_camera->set_view(
            sun_directional_light->transform().get_translation(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Nvidia implementation of view frustrum culling to only show whats in camera view.
        // TODO: Bugfix: This may not be working correctly its worth investigating further
        glm::mat4 cam_view_matrix = camera->get_view_matrix();
        glm::mat4 cam_proj_matrix = camera->get_projection_matrix();
        glm::mat4 inverse_cam_view_matrix = glm::inverse(cam_view_matrix);
        glm::mat4 light_view_matrix = shadow_camera->get_view_matrix();

        const glm::mat4 cam_inverse_matrix = glm::inverse(cam_proj_matrix * cam_view_matrix);

        std::vector<glm::vec4> frustum_points;

        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt = cam_inverse_matrix * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustum_points.push_back(pt / pt.w);
                }
            }
        }

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();
        
        for (glm::vec4 &vec: frustum_points) {
            glm::vec4 vl = light_view_matrix * vec;  // light space

            minX = std::min(minX, vl.x);
            maxX = std::max(maxX, vl.x);
            minY = std::min(minY, vl.y);
            maxY = std::max(maxY, vl.y);
            minZ = std::min(minZ, vl.z);
            maxZ = std::max(maxZ, vl.z);
        }

        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        shadow_camera.reset(
            new OrthographicCamera {minX, maxX, minY, maxY, minZ, maxZ}
        );
        shadow_camera->set_view(
            sun_directional_light->transform().get_translation(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // ===== SHADOW MAP GENERATION =====
        // --- DIRECTION SHADOWS

        depth_map_shader->bind();
        shadow_map->bind();
        shadow_map_buffer->bind();

        Renderer::begin_scene(shadow_camera, { glm::vec4(1.0f), shadow_map_width, shadow_map_height });

        for (auto& [name, object]: m_objects) {
            bool result = object->attached(Object::MESH);
            if (object->attached(Object::MESH) && object->mesh()->draw) {
                Renderer::submit_entity(depth_map_shader, object->mesh(), object->transform());
            }
        }
        depth_map_shader->unbind();
        shadow_map_buffer->unbind();
        shadow_map->unbind();

        // --- POINT SHADOWS
        // TODO: Bugfix: This is not working correctly
        
        shadow_point_map->bind();
        depth_map_shader->bind();
        shadow_point_map_buffer->bind();

        for (unsigned int i = 0; i < 6; i++) {

            lookat_up view = point_shadow_views.at(i);

            // TODO: Optimisation: These can be processed in one render pass instead of 6 - can use a geometry shader to accomplish this.
            shadow_point_camera->set_view(
                point_light_green->transform().get_translation(), view.look_at + point_light_green->transform().get_translation(), view.up
            );
            //Set the cubemap surface to draw to.
            shadow_point_map->set_data(i);
            Renderer::begin_scene(shadow_point_camera, { glm::vec4(1.0f), shadow_map_width, shadow_map_height });

            //Draw the objects
            // TODO: Optimisation: Add a depth check on objects to see if they are in range of the light otherwise don't draw them.
            for (auto& [name, object]: m_objects) {
                bool result = object->attached(Object::MESH);
                if (object->attached(Object::MESH) && object->mesh()->draw) {
                    Renderer::submit_entity(depth_map_shader, object->mesh(), object->transform());
                }
            }
        }

        depth_map_shader->unbind();
        shadow_point_map_buffer->unbind();
        shadow_point_map->unbind();


        // ===== END SHADOW MAP GENERATION =====

        texture_shader->bind();
        int light_counter = 0;
        for (auto& [name, object]: m_objects) {
            if (object->attached(Object::LIGHT)) {
                std::string u_light_name = "u_lights[" + std::to_string(light_counter) + "]";
                // TODO: Get a final transform (object + light) to get a position in a better way
                glm::vec3 obj_pos = object->transform().get_translation();
                texture_shader->upload_u_vec3(u_light_name + ".position", object->light().position + obj_pos);
                texture_shader->upload_u_vec3(u_light_name + ".color", object->light().get_hdr_color());
                texture_shader->upload_u_int1(u_light_name + ".cast_shadows", object->light().cast_shadows);
                texture_shader->upload_u_int1(u_light_name + ".enabled", object->light().enabled);
                texture_shader->upload_u_int1(u_light_name + ".direction", object->light().direction);
                light_counter += 1;
            }
        }

        texture_shader->upload_u_vec3("u_camera_position", camera->get_position());
        texture_shader->upload_u_int1("u_render_mode", render_mode);
        texture_shader->upload_u_int1("u_cast_shadows", cast_shadows);

        width = Application::get().get_window().get_width();
        height = Application::get().get_window().get_height();
        if (width != Application::get().get_window().get_width()
            || height != Application::get().get_window().get_height()
        ) {
            width = Application::get().get_window().get_width();
            height = Application::get().get_window().get_height();
            float aspect = (float)width / (float)height;
        }
        if (use_shadow_cam) {
            current_camera = shadow_camera;
            Renderer::begin_scene(shadow_camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, shadow_map_width, shadow_map_height });
        } else if (use_debug_cam) {
            current_camera = debug_camera;
            Renderer::begin_scene(debug_camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, width, height });
        } else {
            current_camera = camera;
            Renderer::begin_scene(camera, { glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}, width, height });
        }
        texture_shader->upload_u_mat4("u_light_space_matrix", shadow_camera->get_view_projection_matrix());
        shadow_map->bind(texture_shader->uniform_texture_unit("shadow_map"));
        shadow_point_map->bind(texture_shader->uniform_texture_unit("point_light_shadow_map"));

        irradiance_map->bind(texture_shader->uniform_texture_unit("irradiance_map"));
        prefilter_map->bind(texture_shader->uniform_texture_unit("prefilter_map"));
        if (use_generated_brdf) {
            brdf_map->bind(texture_shader->uniform_texture_unit("brdf_map"));
        } else {
            brdf_map_file->bind(texture_shader->uniform_texture_unit("brdf_map"));
        }

        for (auto& [name, object]: m_objects) {
            if (object->attached(Object::MESH) && object->mesh()->draw) {
                Renderer::submit_entity(texture_shader, object->mesh(), object->transform());
            }
        }

        // Draw light positions
        for (auto& [name, object]: m_objects) {
            if (object->attached(Object::LIGHT)) {
                if (object->light().enabled) {
                    glm::vec3 obj_pos = object->transform().get_translation();
                    Transform t = { object->light().position + obj_pos, glm::vec3(0.05, 0.05, 0.05), glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };
                    engine_debug::draw_cube(t, glm::vec4(object->light().color, 1.0f));
                }
            }
        }

        if (use_debug_cam) {

            Transform t = { cam_center, glm::vec3(0.1, 0.1, 0.1), glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };
            engine_debug::draw_cube(t, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            engine_debug::draw_line(glm::vec3(frustum_points[0]), glm::vec3(frustum_points[1]), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[2]), glm::vec3(frustum_points[3]), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[4]), glm::vec3(frustum_points[5]), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[6]), glm::vec3(frustum_points[7]), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            engine_debug::draw_line(glm::vec3(frustum_points[1]), glm::vec3(frustum_points[3]), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[3]), glm::vec3(frustum_points[7]), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[7]), glm::vec3(frustum_points[5]), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
            engine_debug::draw_line(glm::vec3(frustum_points[5]), glm::vec3(frustum_points[1]), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

            engine_debug::draw_line(
                shadow_camera->get_position() + glm::vec3(minX, minY, maxZ),
                shadow_camera->get_position() + glm::vec3(minX, maxY, maxZ),
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );
            engine_debug::draw_line(
                shadow_camera->get_position() + glm::vec3(minX, minY, minZ),
                shadow_camera->get_position() + glm::vec3(minX, maxY, minZ),
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );
            engine_debug::draw_line(
                shadow_camera->get_position() + glm::vec3(minX, minY, minZ),
                shadow_camera->get_position() + glm::vec3(minX, maxY, minZ),
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );
            engine_debug::draw_line(
                shadow_camera->get_position() + glm::vec3(maxX, minY, minZ),
                shadow_camera->get_position() + glm::vec3(maxX, maxY, minZ),
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );
            engine_debug::draw_line(
                shadow_camera->get_position() + glm::vec3(maxX, minY, maxZ),
                shadow_camera->get_position() + glm::vec3(maxX, maxY, maxZ),
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );


            // Draw shadow cam boundaries
            engine_debug::draw_line({ minX, minY, minZ }, { maxX, maxY, minZ }, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            engine_debug::draw_line({ maxX, minY, minZ }, { minX, maxY, minZ }, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            engine_debug::draw_line({ minX, minY, maxZ }, { maxX, maxY, maxZ }, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            engine_debug::draw_line({ maxX, minY, maxZ }, { minX, maxY, maxZ }, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

            glm::vec3 shadow_cam_pos = shadow_camera->get_position();
            glm::vec3 shadow_cam_lookat = shadow_camera->get_look_at();

            engine_debug::draw_line(shadow_cam_pos, shadow_cam_lookat, glm::vec4(1.0f, 0.0f, 0.3f, 1.0f));

            engine_debug::draw_deferred();
        }

        // Draw shadow camera
        /*
        if (use_debug_cam) {
            Transform t = { {0.0f, 0.0f, 0.0f}, glm::vec3(1.0f, 1.0f, 1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };
            engine_debug::draw_cube_deferred(t, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            ENGINE_INFO("camera - {0}, {1}, {2}", minX, minY, minZ);
            engine_debug::draw_deferred();
        }*/

        for (auto& [name, object]: m_objects) {
            if (object->collider() != nullptr) {
                if (object->collider()->debug_draw_enabled) {
                    engine_debug::draw_box_collider(*object->collider());
                }
            }
        }

        engine_debug::draw_simple_line(
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
        );
        engine_debug::draw_simple_line(
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
        );
        engine_debug::draw_simple_line(
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
        );

        engine_debug::draw_buffers();

        if (render_skybox) {
            skybox_shader->bind();
            if (skybox_mode == 0) environment_map->bind(skybox_shader->uniform_texture_unit("environment_map"));
            else if (skybox_mode == 1) irradiance_map->bind(skybox_shader->uniform_texture_unit("environment_map"));
            else if (skybox_mode == 2) prefilter_map->bind(skybox_shader->uniform_texture_unit("environment_map"));
            else if (skybox_mode == 3) shadow_point_map->bind(skybox_shader->uniform_texture_unit("environment_map"));
            skybox_shader->upload_u_mat4("u_view", camera->get_view_matrix());
            skybox_shader->upload_u_mat4("u_projection", camera->get_projection_matrix());
            Renderer::submit(skybox_shader, cube_vao, glm::mat4(1.0f));
        }

        for (auto& [name, entity]: m_entities) {
            if (auto gltf_entity = std::dynamic_pointer_cast<GltfEntity>(entity)) {
                gltf_entity->clear_gltf_data();
            }
        }
    }

private:
    std::shared_ptr<PythonScript> py_script;

    std::shared_ptr<FrameBuffer> fbo_filter;

    std::shared_ptr<Shader> ibl_equi_to_cube_shader;
    std::shared_ptr<Shader> convolution_shader;
    std::shared_ptr<Shader> prefilter_shader;
    std::shared_ptr<Shader> brdf_shader;
    std::shared_ptr<Shader> skybox_shader;

    std::shared_ptr<Shader> texture_shader;
    std::shared_ptr<Shader> depth_map_shader;

    std::shared_ptr<NewPerspectiveCamera> camera;
    std::shared_ptr<NewPerspectiveCamera> debug_camera;
    std::shared_ptr<PerspectiveCamera> shadow_point_camera;
    std::shared_ptr<Camera> ibl_camera;
    std::shared_ptr<OrthographicCamera> shadow_camera;
    std::shared_ptr<Camera> current_camera;

    std::shared_ptr<VertexArray> cube_vao;

    std::shared_ptr<Texture> checker_texture;
    std::shared_ptr<Texture> dirt_rma_texture;
    std::shared_ptr<Texture> dirt_albedo_texture;
    std::shared_ptr<Texture> dirt_normal_texture;
    std::shared_ptr<Texture> hdr_map;
    std::shared_ptr<Texture> brdf_map;
    std::shared_ptr<Texture> brdf_map_file;

    std::shared_ptr<TextureDepth> shadow_map;
    std::shared_ptr<FrameBuffer> shadow_map_buffer;

    std::shared_ptr<TextureCubeMap> shadow_point_map;
    std::shared_ptr<FrameBuffer> shadow_point_map_buffer;

    std::shared_ptr<TextureCubeMap> environment_map;
    std::shared_ptr<TextureCubeMap> irradiance_map;
    std::shared_ptr<TextureCubeMap> prefilter_map;

    std::shared_ptr<Object> sun_directional_light;
    std::shared_ptr<Object> point_light_green;

    glm::mat4 model_matrix {1.0f};
    glm::vec3 model_position {0.0f, -2.0f, 0.0f};
    //glm::vec3 light_position {3, 3, 3};
    glm::vec3 light_color {0.01, 0.01, 0.01};
    //glm::vec3 light_position_b {2, 0, 0};
    glm::vec3 light_color_b {0.2, 0.1, 0.1};
    glm::vec3 light_position_c {-2, 0, 0};
    glm::vec3 light_color_c {0.1, 0.2, 0.1};

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
    bool use_debug_cam = false;
    bool render_skybox = true;
    bool cast_shadows = true;
    bool use_generated_brdf = true;

    int render_mode = 0;
    int skybox_mode = 1;

    int width = 1920;
    int height = 1080;

    int shadow_map_width = 4096;
    int shadow_map_height = 4096;

    // Window flags
    bool show_entity_window = true;
    bool show_scene_window = true;

    bool run_once = false;
};

int main() {
    engine::Application application{1920, 1080};
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}