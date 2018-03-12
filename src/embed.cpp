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
#include <imgui.h>
#include "graphics/imgui_impl_glfw_gl3.h"

#include "python_api.hpp"
#include "tracker.hpp"
#include "graphics/arcball.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/cube.hpp"
#include "graphics/buffers.hpp"
#include "graphics/console.hpp"
#include "graphics/model.hpp"
#include "graphics/debug.hpp"
#include "graphics/shapefile_loader.hpp"
#include "graphics/planes.hpp"
#include "graphics/renderer.hpp"

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

glm::mat4 Projection = glm::mat4(1.0f);

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

ImGuiIO& io = ImGui::GetIO();

void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (io.WantCaptureKeyboard) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
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
    window = renderer.get_window();

    // ------------ Graphics Engine ---------------

    GLuint programID = BuildGlProgram(
        "./src/shaders/vertex_shader.glsl", 
        "./src/shaders/fragment_shader.glsl"
    );
    GLuint sprite_program = BuildGlProgram(
        "./src/shaders/sprite_vertex_shader.glsl", 
        "./src/shaders/fragment_shader.glsl"
    );
    GLuint simple_program = BuildGlProgram(
        "./src/shaders/simple_vertex_shader.glsl",
        "./src/shaders/simple_fragment_shader.glsl"
    );
    GLuint line_program = BuildGlProgram(
        "./src/shaders/line_v_shader.glsl",
        "./src/shaders/line_f_shader.glsl",
        "./src/shaders/line_g_shader.glsl"
    );

    Shader base_shader(programID);

    DebugInit();
    glUseProgram(programID);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    glViewport(0, 0, width, height);

    GLint size;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
    std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE is " << size << " bytes." << endl;

    // Mesh Objects
    GLuint CubeMesh = BufferMeshDataVNT(cube_data_normal, sizeof(cube_data_normal));
    GLuint LightMesh = BufferMeshDataVT(cube_data, sizeof(cube_data));

    // Load Textures
    GLuint texture = BufferTextureDataFromFile("container.jpg", "./assets/textures/");

    struct DrawObject {
        GLuint mesh_id;
        GLuint tex_id;
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scale;
        GLuint program;
        const char *name;
    };

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

    // Create our Objects
    DrawObject drawObjects[] = {
        { CubeMesh, texture, glm::vec3(-1, 0.5, -1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube1"},
        { CubeMesh, texture, glm::vec3(-2, 0.5, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube2" },
        { CubeMesh, texture, glm::vec3(-1, 1.5, -2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube3" },
        { LightMesh, NULL_TEXTURE, lightPos, glm::vec3(0, 0, 0), glm::vec3(0.25, 0.25, 0.25), simple_program, "Light1" }
    };

    LoadShapeFile("./assets/shapefiles/Australia.shp");

    GLuint MVPMatID = glGetUniformLocation(programID, "MVP");
    GLuint normalMatID = glGetUniformLocation(programID, "NormalMat");
    GLuint modelMatId = glGetUniformLocation(programID, "Model");
    GLuint projMatId = glGetUniformLocation(programID, "Projection");
    GLuint viewMatId = glGetUniformLocation(programID, "View");
    GLint objectColorLoc = glGetUniformLocation(programID, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(programID, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(programID, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(programID, "viewPos");

    ImGui_ImplGlfwGL3_Init(window, true);
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("assets/fonts/calibri.ttf", 15.0f);

    // Register Key callbacks
    glfwSetKeyCallback(window, key_handler);
    glfwSetMouseButtonCallback(window, mouseButtonCallback2);
    glfwSetCursorPosCallback(window, mouseCursorCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetScrollCallback(window, scrollCallback);


    int num_lines = 10;
    int line_point_count = 200;

    for (int x=0; x < num_lines; x++) {
        random_line(line_point_count);
    }
    
    build_line_data();

    // TestInput
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        float position = glfwGetTime() * 1.0f;
    }

    // Main Loop
    std::cout << "Tester" << std::endl;
    std::cout << DebugFlagList() << std::endl; 
    glClearColor(0.0f, 0.25f, 0.25f, 0.0f);
    float last_frame_time = glfwGetTime();

    do {
        float current_frame_time = glfwGetTime();
        float delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        glfwPollEvents();
        //ImGuiIO& io = 
        ImGui::GetIO();

        //set_ortho_pos(glm::vec3(mouse_world_coords.x, mouse_world_coords.y, 0.0f));
        new_mouse_world_coords = calc_world_coords();

        if (middle_button_down) { 
            //new_mouse_world_coords = calc_world_coords();

            glm::vec2 difference = new_mouse_world_coords - mouse_world_coords;
            difference = glm::vec2(difference.x, difference.y);
            translate_ortho(glm::vec3(difference, 0.0f));
            //set_ortho_pos(glm::vec3(mouse_world_coords.x, mouse_world_coords.y, 0.0f));
            mouse_world_coords = new_mouse_world_coords;
        }

        ImGui_ImplGlfwGL3_NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Do camera changes here before we start drawing
        glm::mat4 rotated_view = getView();

        glUseProgram(programID);

        MVPMatID = glGetUniformLocation(programID, "MVP");
        normalMatID = glGetUniformLocation(programID, "NormalMat");
        modelMatId = glGetUniformLocation(programID, "Model");
        projMatId = glGetUniformLocation(programID, "Projection");
        viewMatId = glGetUniformLocation(programID, "View");
        objectColorLoc = glGetUniformLocation(programID, "objectColor");
        lightColorLoc = glGetUniformLocation(programID, "lightColor");
        lightPosLoc = glGetUniformLocation(programID, "lightPos");
        viewPosLoc = glGetUniformLocation(programID, "viewPos");

        glUniformMatrix4fv(projMatId, 1, GL_FALSE, &Projection[0][0]);
        glUniformMatrix4fv(viewMatId, 1, GL_FALSE, &rotated_view[0][0]);

        for (uint i = 0; i < sizeof(drawObjects) / sizeof(DrawObject); i++) {
            if (strcmp("Light1", drawObjects[i].name) == 0) {
                float x = (float)glm::sin(glfwGetTime()) * 3;
                lightPos.x = x;
                drawObjects[i].pos.x = x;
            }
            glUseProgram(drawObjects[i].program);
            //recalculate the mvp
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, drawObjects[i].pos);
            //model = glm::rotate(model, 45.0f, drawObjects[i].rot);
            model = glm::scale(model, drawObjects[i].scale);
            glm::mat4 mvp = Projection * rotated_view * model;
            glm::mat3 normalMat = (glm::mat3)glm::transpose(glm::inverse(model));
            // Load camera to OpenGL
            MVPMatID = glGetUniformLocation(drawObjects[i].program, "MVP");
            normalMatID = glGetUniformLocation(drawObjects[i].program, "NormalMat");
            modelMatId = glGetUniformLocation(drawObjects[i].program, "Model");
            glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
           
            if (drawObjects[i].program == programID) {
                glUniformMatrix4fv(modelMatId, 1, GL_FALSE, &model[0][0]);
                glUniformMatrix3fv(normalMatID, 1, GL_FALSE, &normalMat[0][0]);
                glUniform1i(glGetUniformLocation(programID, "debug_draw_normals"),
                    DebugGetFlag("render:draw_normals"));
                glUniform1i(glGetUniformLocation(programID, "debug_draw_texcoords"),
                    DebugGetFlag("render:draw_texcoords"));
                glUniform1i(glGetUniformLocation(programID, "debug_disable_lighting"),
                    DebugGetFlag("render:disable_lighting"));
                glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
                glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Also set light's color (white)
                glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
            }
            if (drawObjects[i].tex_id != NULL_TEXTURE) {
                GLint tex_loc = glGetUniformLocation(drawObjects[i].program, "texture_diffuse1");
                glUniform1i(tex_loc, 0);
                glBindTexture(GL_TEXTURE_2D, drawObjects[i].tex_id);
            }
            if (drawObjects[i].program == simple_program) {
                glUniform1i(glGetUniformLocation(simple_program, "use_uniform_color"), true);
                glUniform3f(glGetUniformLocation(simple_program, "uniform_color"),
                    1.0f, 1.0f, 1.0f);
            }
            glBindVertexArray(drawObjects[i].mesh_id);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }

        glUseProgram(simple_program);
        glUniform1i(glGetUniformLocation(simple_program, "use_uniform_color"), false);
        glm::mat4 plane_model = glm::mat4(1.0f);
        rotated_view = getView();
        //model = glm::translate(model, drawObjects[i].pos);
        //model = glm::rotate(model, 45.0f, drawObjects[i].rot);
        //model = glm::scale(model, drawObjects[i].scale);
        glm::mat4 mvp = Projection * rotated_view * plane_model;
        //normalMat = (glm::mat3)glm::transpose(glm::inverse(model));
        // Load camera to OpenGL
        MVPMatID = glGetUniformLocation(simple_program, "MVP");
        normalMatID = glGetUniformLocation(simple_program, "NormalMat");
        modelMatId = glGetUniformLocation(simple_program, "Model");
        glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);

        draw_shapes();

        // ========= SPRITE DRAWING =========
        
        glUseProgram(sprite_program);
 
        //MVPMatID = glGetUniformLocation(sprite_program, "MVP");
        //normalMatID = glGetUnicharformLocation(sprite_program, "NormalMat");
        projMatId = glGetUniformLocation(sprite_program, "Projection");
        viewMatId = glGetUniformLocation(sprite_program, "View");
        objectColorLoc = glGetUniformLocation(sprite_program, "objectColor");
        lightColorLoc = glGetUniformLocation(sprite_program, "lightColor");
        lightPosLoc = glGetUniformLocation(sprite_program, "lightPos");
        viewPosLoc = glGetUniformLocation(sprite_program, "viewPos");

        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Also set light's color (white)

        glUniform1i(glGetUniformLocation(sprite_program, "debug_draw_normals"),
            DebugGetFlag("render:draw_normals"));
        glUniform1i(glGetUniformLocation(sprite_program, "debug_draw_texcoords"),
            DebugGetFlag("render:draw_texcoords"));
        glUniform1i(glGetUniformLocation(sprite_program, "debug_disable_lighting"),
            DebugGetFlag("render:disable_lighting"));

        glUniformMatrix4fv(projMatId, 1, GL_FALSE, &Projection[0][0]);
        glUniformMatrix4fv(viewMatId, 1, GL_FALSE, &rotated_view[0][0]);
        //glm::mat4 model = glm::mat4(1.0f);
        // Need to get correct 

        //glm::mat4 model_mvp = Projection * rotated_view * model;
        //glm::mat3 model_normalMat = (glm::mat3)glm::transpose(glm::inverse(model));
        //glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &model_mvp[0][0]);
        //glUniformMatrix3fv(normalMatID, 1, GL_FALSE, &model_normalMat[0][0]);
        //GLint tex_loc = glGetUniformLocation(sprite_program, "texture_diffuse1");
        //glUniform1i(tex_loc, 0);

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
        
        // ========= END SPRITE DRAWING =========

        DebugDraw(Projection, rotated_view);

        // ========= LINE DRAWING =========
        glUseProgram(line_program);
        MVPMatID = glGetUniformLocation(line_program, "MVP");
        glm::mat4 model = glm::mat4(1.0f);
        mvp = Projection * rotated_view * model;
        glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
        draw_lines();

        static bool p_open = true;
        ShowExampleAppConsole(&p_open);
        ShowFrameInformation(&p_open);
        ShowMainMenu(&p_open);
        MenuParts(&p_open);

        ImGui::Render();
        glfwSwapBuffers(window);
        
    } 
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    // END OPENGL STUFF

    unload_shaders();

    if (Py_FinalizeEx() < 0) {
        exit(120);
    };
    PyMem_RawFree(program);
    return 0;
}
