/*
This is an example C application that has an embedded python interpreter.
It provides C function calls that be called from Python and
Python function calls that can be called from C.
*/

#include <Python.h>
#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include "imgui.h"
#include "graphics/imgui_impl_glfw_gl3.h"

#include "python_api.hpp"
#include "python_manager_api.hpp"
#include "tracker.hpp"
#include "graphics/arcball.hpp"
#include "graphics/camera.hpp"
#include "graphics/buffers.hpp"
#include "graphics/console.hpp"
#include "graphics/model.hpp"
#include "graphics/debug.hpp"
#include "graphics/shapefile_loader.hpp"
#include "graphics/planes.hpp"
#include "graphics/renderer.hpp"

#include "managers/state_manager.hpp"
#include "managers/texture_manager.hpp"
#include "managers/primitive_manager.hpp"
#include "managers/mesh_manager.hpp"

#define NULL_TEXTURE 0

GLFWwindow* window;
int width = 1920;
int height = 1080;
bool debug_draw_normals = false;
bool debug_draw_texcoords = false;
bool debug_disable_lighting = false;

void disable_debugs() {
    debug_draw_normals = false;
    debug_draw_texcoords = false;
    debug_disable_lighting = false;
}

glm::mat4 Projection {1.0f};
glm::mat4 rotated_view {1.0f};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Key checking
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        std::cout << "Toggling Draw Normals - " << std::endl;
        if (debug_draw_normals) {
            std::cout << "Off" << std::endl;
            disable_debugs();
        }
        else {
            std::cout << "On" << std::endl;
            disable_debugs();
            debug_draw_normals = true;
        }
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        std::cout << "Toggling Draw Texure Coordinates - " << std::endl;
        if (debug_draw_texcoords) {
            std::cout << "Off" << std::endl;
            disable_debugs();
        }
        else {
            std::cout << "On" << std::endl;
            disable_debugs();
            debug_draw_texcoords = true;
        }
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        std::cout << "Toggling Draw Lighting - " << std::endl;
        if (debug_disable_lighting) {
            std::cout << "On" << std::endl;
            disable_debugs();
        }
        else {
            std::cout << "Off" << std::endl;
            disable_debugs();
            debug_disable_lighting = true;
        }
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        std::cout << "Toggling Camera" << std::endl;
        toggleCamera();
        Projection = getProj();
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        std::cout << "Panning Up" << std::endl;
        move_up();
        Projection = getProj();
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        std::cout << "Panning Down" << std::endl;
        move_down();
        Projection = getProj();
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        std::cout << "Panning Right" << std::endl;
        move_right();
        Projection = getProj();
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS)) {
        std::cout << "Panning Left" << std::endl;
        move_left();
        Projection = getProj();
    }
    if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS)) {
        std::cout << "zooming in" << std::endl;
        zoom_in();
        Projection = getProj();
    }
    if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS)) {
        std::cout << "Zooming out" << std::endl;
        zoom_out();
        Projection = getProj();
    }
}

//ImGuiIO& io = ImGui::GetIO();

void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    } else {
        key_callback(window, key, scancode, action, mods);
    }
}

// Test mouse controls
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int arcball_on = false;
int zoom = 0;

bool middle_button_down = false;
glm::vec2 mouse_world_coords = glm::vec2(1.0f);
glm::vec2 new_mouse_world_coords = glm::vec2(1.0f);
glm::vec2 previous_mouse_position = glm::vec2(0.0f, 0.0f);
glm::vec3 start_ortho_pos = glm::vec3(1.0f);

glm::vec2 calc_world_coords() {
    // Calculate mouse world coordinates
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double dxpos, dypos;
    glfwGetCursorPos(window, &dxpos, &dypos);
    float xpos = (float)dxpos;
    float ypos = (float)dypos;

    if (xpos < 0.0f) {
        xpos = 0.0f;
    }
    if (ypos < 0.0f) {
        ypos = 0.0f;
    }

    glm::mat4 view_mat = getView(start_ortho_pos);
    glm::mat4 view = Projection * view_mat;

    glm::vec3 near = glm::unProject( glm::vec3(xpos, ypos, 0.0f), glm::mat4(1.0f), view, glm::vec4(0, 0, width, height));
    glm::vec3 far = glm::unProject( glm::vec3(xpos, ypos, 1.0f), glm::mat4(1.0f), view, glm::vec4(0, 0, width, height));

    glm::vec4 world_coords = glm::vec4(near.x, near.y, 0.0f, 1.0f);
    set_mouse_world_pos(world_coords.x, world_coords.y * -1);
    return glm::vec2(world_coords.x, world_coords.y * -1);
}

// glfwSetMouseButtonCallback(window, mouse_button_callback);
void mouseButtonCallback2( GLFWwindow * window, int button, int action, int mods ){
    mouseButtonCallback(window, button, action, mods);

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            middle_button_down = true;
            start_ortho_pos = get_ortho_pos();
            mouse_world_coords = calc_world_coords();
            //set_ortho_pos(glm::vec3(mouse_world_coords.x, mouse_world_coords.y, 0.0f));
            
            std::cout << "MOUSE BUTTON RIGHT DOWN, " << middle_button_down << std::endl;

        } else if (action == GLFW_RELEASE) {
            // Now we need to set our camera to our current position.
            middle_button_down = false;
            std::cout << "MOUSE BUTTON RIGHT RELEASED, " << middle_button_down << std::endl;
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom += yoffset;
    if (yoffset > 0.0) {
        zoom_in();
    } else {
        zoom_out();
    }
}

void resizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    std::cout << "Width: " << width << " Height: " << height << std::endl;
    glViewport(0, 0, width, height);
    Projection = cameraUpdate(width, height);
}

wchar_t *program;

int pythonTesting(int argc, char *argv[]) {
    // Python stuff below
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        std::cout << "Fatal error: cannot decode argv[0] (locale)" << std::endl;
        std::runtime_error("Arguemnt error");
    }
    Py_SetProgramName(program);
    PyImport_AppendInittab("debug", &PyInit_Debug);
    PyImport_AppendInittab("planes", &PyInit_Planes);
    PyImport_AppendInittab("engine_console", &PyInit_console_engine);
    PyImport_AppendInittab("manager", &PyInit_manager);
    Py_Initialize();
    PyRun_SimpleString(
        "import sys;"
        "sys.path.append('./')"
    );
    pName = PyUnicode_FromString("console.sample");
    pModule = PyImport_Import(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "print_random_word");
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_Pack(1, Py_BuildValue("i", 10));
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            Py_DECREF(pValue);
        } else {
            PyErr_Print();
            std::cout << "Failed to find function - print_random_word" << std::endl;
            return 1;            
        }
    } else {
        PyErr_Print();
        std::cout << "Failed to load module - console.sample" << std::endl;
        return 1;
    }
    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    PyRun_SimpleString(
        "from time import time;"
        "the_time = time();"
        "print(f'Time is {the_time}');"
    );
    return 0;
}

void random_line(int num_lines) {
    const long uid = new_line();
    int start_x = (rand() % 360) - 180;
    int start_y = (rand() % 180) - 90;
    for (int x = 0; x <= num_lines; x++){
        insert_into_line(uid, start_x, start_y);
        start_x += (rand() % 10) - 5;
        start_y += (rand() % 10) - 5;
    }
}

int main(int argc, char *argv[]) {

    pythonTesting(argc, argv);

    std::cout << "Initialising renderer" << std::endl;
    Renderer renderer;
    renderer.create();
    window = renderer.get_window();

    // ------------ Graphics Engine ---------------

    GLuint programID, sprite_program, simple_program, line_program;
    renderer.LoadShaders(&programID, &sprite_program, &simple_program, &line_program);

    //Shader base_shader(programID);

    DebugInit();
    renderer.activate("default");

    // Mesh Objects
    GLuint CubeMesh = primitives::cube_mesh();
    GLuint LightMesh = primitives::light_mesh();

    // Load Textures
    TextureManager texture_manager(renderer);
    texture_manager.add_texture("wooden_crate", "container.jpg", "./assets/textures/");
    texture_manager.add_texture("wooden_floor", "light_wood_floor.jpg", "./assets/textures/");

    glm::vec3 lightPos = glm::vec3(3.0f, 2.0f, 0.0f);
    glm::vec3 viewPos = glm::vec3(7, 3, 6);

    // Set up Cameras
    cameraUpdate(width, height);
    Projection = getProj();

    InitPlanes(sprite_program);

    int player_index = UpdatePlaneMatrix("player", glm::mat4(1.0f));
    int player_index2 = UpdatePlaneMatrix("player2", glm::mat4(1.0f));

    int result = UpdatePlaneBuffers(1, 1, "grass", glm::vec2(1, 1), player_index);
    result = UpdatePlaneBuffers(4, 4, "rock", glm::vec2(4, 4), player_index2);
    result = UpdatePlaneBuffers(2, 2, "sand", glm::vec2(2, 2));
    result = UpdatePlaneBuffers(0.5, 0.5, "dirt", glm::vec2(0.5, 0.5));
    if (result > 0) {
        std::cout << "Error updating plane buffers" << std::endl;
    }
    UpdateMatrixBuffer();

    LoadShapeFile("./assets/shapefiles/Australia.shp");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);

    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("assets/fonts/calibri.ttf", 15.0f);

    // Register Key callbacks
    glfwSetKeyCallback(window, key_handler);
    glfwSetMouseButtonCallback(window, mouseButtonCallback2);
    glfwSetCursorPosCallback(window, mouseCursorCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetScrollCallback(window, scrollCallback);

    State state(renderer);
    state.update_state();
    PrimitiveManager primitives{state, texture_manager};
    primitives.new_instance("Cube", "wooden_crate", glm::vec3(-1, 0.5, -1));
    primitives.new_instance("Cube", "wooden_crate", glm::vec3(-2, 0.5, 0));
    unsigned int floor_index = primitives.new_instance("Plane", "wooden_floor", glm::vec3(0, 0, 0));
    primitives.new_instance("Cube", "wooden_crate", glm::vec3(-1, 1.5, -2));
    unsigned int light_index = primitives.new_instance(
        "Cube", glm::vec3(0,2,0), glm::vec3(1,1,1), false
    );
    primitives.update_instance_scale(light_index, glm::vec3(0.25, 0.25, 0.25));
    primitives.update_instance_scale(floor_index, glm::vec3(10, 10, 10));
    
    MeshManager meshes {state, texture_manager};
    unsigned int nano = meshes.new_instance("nanosuit.obj", "./assets/meshes/", glm::vec3(2, 0, -4));
    meshes.update_instance_scale(nano, glm::vec3(0.2, 0.2, 0.2));

    unsigned int warr = meshes.new_instance("warrior.fbx", "./assets/meshes/", glm::vec3(0, 0, 0));
    meshes.update_instance_rotation(warr, glm::vec3(-90.0f, 0, 0));

    assign_managers(state, primitives, meshes);

    int num_lines = 1;
    int line_point_count = 100;

    for (int x=0; x < num_lines; x++) {
        random_line(line_point_count);
    }
    
    build_line_data();

    // TestInput
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        float position = glfwGetTime() * 1.0f;
    }

    // Set up python scripting calls
    PyObject *py_interface_name, *py_interface_module, *pFunc;
    
    py_interface_name = PyUnicode_FromString("engine.interface");
    py_interface_module = PyImport_Import(py_interface_name);
    Py_DECREF(py_interface_name);

    PyObject *py_on_init, *py_on_frame;

    if (py_interface_module != NULL) {
        py_on_init = PyObject_GetAttrString(py_interface_module, "on_init");
        py_on_frame = PyObject_GetAttrString(py_interface_module, "on_frame");
        
        if (!py_on_init && !PyCallable_Check(py_on_init)) {
            PyErr_Print();
            std::cout << "Failed to find function - on_init" << std::endl;
            return 1;            
        }
        if (!py_on_frame && !PyCallable_Check(py_on_frame)) {
            PyErr_Print();
            std::cout << "Failed to find function - on_frame" << std::endl;
            return 1;            
        }
    } else {
        PyErr_Print();
        std::cout << "Failed to load module - engine.objects" << std::endl;
        return 1;
    }

    PyObject *py_args, *py_return;

    py_return = PyObject_CallObject(py_on_init, NULL);
    Py_DECREF(py_return);


    // Main Loop
    glClearColor(0.0f, 0.25f, 0.25f, 0.0f);
    float last_frame_time = glfwGetTime();

    do {
        float current_frame_time = glfwGetTime();
        float delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        py_args = PyTuple_Pack(1, Py_BuildValue("f", delta_time));
        py_return = PyObject_CallObject(py_on_frame, py_args);
        Py_DECREF(py_args);
        if (py_return == NULL) {
            PyErr_Print();
        } else {
            Py_DECREF(py_return);
        }

        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        new_mouse_world_coords = calc_world_coords();

        if (middle_button_down) { 
            glm::vec2 difference = new_mouse_world_coords - mouse_world_coords;
            difference = glm::vec2(difference.x, difference.y);
            translate_ortho(glm::vec3(difference, 0.0f));
            mouse_world_coords = new_mouse_world_coords;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Do camera changes here before we start drawing
        rotated_view = getView();

        // ========= MODEL DRAWING =========

        renderer.activate("default");

        float x = (float)glm::sin(glfwGetTime()) * 3;
        lightPos.x = x;
        glm::vec3 light_pos = primitives.get_instance_position(light_index);
        primitives.update_instance_position(light_index, glm::vec3(x, light_pos.y, light_pos.z));

        state.set_projection(Projection);
        state.set_view(rotated_view);
        state.set_light_pos(lightPos);
        state.set_view_pos(viewPos);
        state.update_state();

        primitives.draw();
        meshes.draw();

        // ========= MODEL DRAWING =========

        // ========= SHAPE DRAWING =========
        {
            glUseProgram(simple_program);
            glUniform1i(glGetUniformLocation(simple_program, "use_uniform_color"), false);
            glm::mat4 plane_model = glm::mat4(1.0f);
            rotated_view = getView();

            glm::mat4 mvp = Projection * rotated_view * plane_model;
            // Load camera to OpenGL
            renderer.active().set_uniform("MVP", mvp);

            draw_shapes();
        }
        // ========= END SHAPE DRAWING =========

        // ========= SPRITE DRAWING =========
        {
            renderer.activate("sprite");
            
            renderer.active().set_uniform("lightPos", lightPos);
            renderer.active().set_uniform("viewPos", viewPos);
            renderer.active().set_uniform("objectColor", 1.0f, 1.0f, 1.0f);
            renderer.active().set_uniform("lightColor", 1.0f, 1.0f, 1.0f);

            renderer.active().set_uniform("debug_draw_normals", DebugGetFlag("render:draw_normals"));
            renderer.active().set_uniform("debug_draw_texcoords", DebugGetFlag("render:draw_normals"));
            renderer.active().set_uniform("debug_disable_lighting", DebugGetFlag("render:draw_normals"));
            
            renderer.active().set_uniform("Projection", Projection);
            renderer.active().set_uniform("View", rotated_view);

            glm::mat4 player_mat = GetPlaneMatrix("player");
            float x = (float)glm::sin(glfwGetTime()) * 0.01;
            player_mat = glm::translate(player_mat, vec3(0, x, 0));
            UpdatePlaneMatrix("player", player_mat);

            player_mat = GetPlaneMatrix("player2");
            x = (float)glm::sin(glfwGetTime()) * 0.01;
            player_mat = glm::translate(player_mat, vec3(x, 0, 0));
            UpdatePlaneMatrix("player2", player_mat);

            UpdateMatrixBuffer();
            DrawPlanes(sprite_program);
        }
        // ========= END SPRITE DRAWING =========

        // ========= DEBUG DRAWING =========
        DebugDraw(Projection, rotated_view);
        // ========= END DEBUG DRAWING =========

        // ========= LINE DRAWING =========
        {
            glUseProgram(line_program);
            renderer.activate("line");
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 mvp = Projection * rotated_view * model;
            renderer.active().set_uniform("MVP", mvp);
            draw_lines();
        }
        // ========= ENDLINE DRAWING =========

        // ========= INTERFACE DRAWING =========
        {
            static bool p_open = true;
            ShowPyEngineConsole(&p_open);
            ShowFrameInformation(&p_open);
            ShowMainMenu(&p_open);
            MenuParts(&p_open);

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        }
        // ========= END INTERFACE DRAWING =========

        glfwSwapBuffers(window);
    } 
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    Py_DECREF(py_on_init);
    Py_DECREF(py_on_frame);
    Py_DECREF(py_interface_module);

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    // END OPENGL STUFF

    unload_shaders();

    if (Py_FinalizeEx() < 0) {
        exit(120);
    };
    PyMem_RawFree(program);
    return 0;
}
