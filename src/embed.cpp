/*
This is an example C application that has an embedded python interpreter.
It provides C function calls that be called from Python and
Python function calls that can be called from C.
*/

#include <Python.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include <imgui.h>
#include "graphics/imgui_impl_glfw_gl3.h"

#include "python_api.hpp"
#include "graphics/arcball.hpp"
#include "graphics/shader.hpp"
#include "graphics/cube.hpp"
#include "graphics/buffers.hpp"
#include "graphics/console.hpp"
#include "graphics/model.hpp"
#include "graphics/debug.hpp"
//#include "graphics/text.hpp"

GLFWwindow* window;
int width = 1024;
int height = 768;
bool debug_draw_normals = false;
bool debug_draw_texcoords = false;
bool debug_disable_lighting = false;

void disable_debugs() {
    debug_draw_normals = false;
    debug_draw_texcoords = false;
    debug_disable_lighting = false;
}

static Arcball arcball(width, height, 0.5f, true, true);
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Key checking
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        printf("Toggling Draw Normals - ");
        if (debug_draw_normals) {
            printf("Off\n");
            disable_debugs();
        }
        else {
            printf("On\n");
            disable_debugs();
            debug_draw_normals = true;
        }
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        printf("Toggling Draw Texure Coordinates - ");
        if (debug_draw_texcoords) {
            printf("Off\n");
            disable_debugs();
        }
        else {
            printf("On\n");
            disable_debugs();
            debug_draw_texcoords = true;
        }
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        printf("Toggling Draw Lighting - ");
        if (debug_disable_lighting) {
            printf("On\n");
            disable_debugs();
        }
        else {
            printf("Off\n");
            disable_debugs();
            debug_disable_lighting = true;
        }
    }

}

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam)
{
    // ignore non-significant error/warning codes
    // Message 131184 displays Buffer memory info,
    // TODO: Load this data into a debug message window
    if (id == 131184) return;

    if (id == 131185) return; // This details VBO allocations (and size)
    if (id == 131204) return; // Texture state usage warning: Texture 0 is base level inconsistent. Check texture size.
    // if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "---------------" << std::endl;
}

// Test mouse controls
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int arcball_on = false;

// glfwSetMouseButtonCallback(window, mouse_button_callback);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    arcball.mouseButtonCallback(window, button, action, mods);
}

// glfwSetCursorPosCallback(window, cursor_pos_callback);
void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos) {
    arcball.cursorCallback(window, xpos, ypos);
}

void resizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    printf("Width: %i, Height: %i", width, height);
    glViewport(0, 0, width, height);
    Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

int main(int argc, char *argv[]) {
    
    #pragma region "Python Code"
    // Python stuff below
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0] (locale)\n");
        exit(1);
    }
    Py_SetProgramName(program);
    PyImport_AppendInittab("pyengine", &PyInit_PyEngine);
    Py_Initialize();
    PyRun_SimpleString(
        "import sys\n"
        "sys.path.append('./')\n"
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
            fprintf(stderr, "Failed to find function - %s", "print_random_word");
            return 1;            
        }
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load module - %s", "console.sample");
        return 1;
    }
    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    PyRun_SimpleString(
        "from time import time\n"
        "the_time = time()\n"
        "print(f'Time is {the_time}')\n"
    );
    /*
    PyRun_SimpleString(
        "import emb\n"
        "print('Result: {}'.format(emb.testfunction(2, 2, 2)))\n"
        "emb.stringfunc('A test c print')\n"
    );
    */
    #pragma endregion
    
    // OPENGL STUFF
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Disable legacy OpenGL
   
    //Debugging 
    printf("DEBUGGING ON");
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    //Create window
    window = glfwCreateWindow(width, height, "Embedded Python", NULL, NULL);
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialise GLEW\n");
        glfwTerminate();
        exit(1);
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // ==== OPENGL START ====
    //
	
	// ------------ Graphics Engine ---------------
    // Turn on Debug callbacks, this should be disabled for RELEASE
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    GLuint programID = BuildGlProgram("./src/shaders/vertex_shader.glsl", 
                                      "./src/shaders/fragment_shader.glsl");
	GLuint simple_program = BuildGlProgram("./src/shaders/simple_vertex_shader.glsl",
										   "./src/shaders/simple_fragment_shader.glsl");
    DebugInit();
    glUseProgram(programID);
	//glEnable(GL_CULL_FACE); // FIX YA NORMALS!
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	glViewport(0, 0, width, height);

	// Mesh Objects
	GLuint CubeMesh = BufferMeshDataVNT(cube_data_normal, sizeof(cube_data_normal));
	GLuint LightMesh = BufferMeshDataVT(cube_data, sizeof(cube_data));

    // Load Textures
    GLuint texture = BufferTextureDataFromFile("container.jpg");

	struct DrawObject {
		GLuint mesh_id;
		GLuint tex_id;
		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 scale;
		GLuint program;
		char *name;
	};

	glm::mat4 cube1_model = glm::mat4(1.0f);
	glm::vec3 lightPos = glm::vec3(3.0f, 2.0f, 0.0f);
	glm::vec3 viewPos = glm::vec3(7, 3, 6);

	// Set up Cameras
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		viewPos, // Light Pos
		glm::vec3(0, 0, 0), // Look At
		glm::vec3(0, 1, 0)  // Up
	);
	glm::mat4 model_mat = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * model_mat;

    // Light Objects
    /*
    Light Types
    - Point light (perspective light with falloff)
        - vec3 position
        - float constant
        - float linear
        - float quadratic
        - vec3 ambient
        - vec3 diffuse
        - vec3 specular

    - Directional Light (orthographic light)
        - vec3 position
        - vec3 direction
        - vec3 ambient
        - vec3 diffuse
        - vec3 specular
    - Spot Light (orthographic with falloff and radius) 
    */
    struct LightObject {

    };


	// Create our Objects
	DrawObject drawObjects[] = {
		{ CubeMesh, texture, glm::vec3(-1, 0.5, -1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube1"},
		{ CubeMesh, texture, glm::vec3(-2, 0.5, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube2" },
		{ CubeMesh, texture, glm::vec3(-1, 1.5, -2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube3" },
		{ LightMesh, NULL, lightPos, glm::vec3(0, 0, 0), glm::vec3(0.25, 0.25, 0.25), simple_program, "Light1" }
	};
    // What information do we need for the model?
    // FilePath
    // FileName
    // Translation
    // Rotation
    // Scale
    
    struct ModelObject {
        glm::vec3 translation;
        GLfloat rotationX;
        GLfloat rotationY;
        GLfloat rotationZ;
        glm::vec3 scale;
        Model model;
    };

    // NOTE: I now need to seperate the Models from the drawing instances ( or not reload a loaded model )
    vector<ModelObject> modelObjects;

    modelObjects.push_back({
        glm::vec3(2, 0, -4),
        0.0f,
        0.0f,
        0.0f,
        glm::vec3(0.2, 0.2, 0.2),
        Model("./assets/meshes/", "nanosuit.obj")
    });

    modelObjects.push_back({
        glm::vec3(0, 0, 0),
        -90.0f,
        0.0f,
        0.0f,
        glm::vec3(0.05, 0.05, 0.05),
        Model("./assets/meshes/", "rook.obj")
    });

	GLuint MVPMatID = glGetUniformLocation(programID, "MVP");
	GLuint normalMatID = glGetUniformLocation(programID, "NormalMat");
	GLuint modelMatId = glGetUniformLocation(programID, "Model");
	GLint objectColorLoc = glGetUniformLocation(programID, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(programID, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(programID, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(programID, "viewPos");

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("assets/fonts/calibri.ttf", 15.0f);
	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

    // Register Key callbacks
    // FIXME: this appears to break my console
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCursorCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    // Main Loop
    glClearColor(0.0f, 0.25f, 0.25f, 0.0f);
    do {    

		glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Do camera changes here before we start drawing
        glm::mat4 rotated_view = View * arcball.createViewRotationMatrix();


        glUseProgram(programID);
        for (uint i = 0; i < modelObjects.size(); i++) {
            // Calculate the matrices
            // TODO: this should be optimised by not recalcuting the Model Matrix if nothing has changed
            glm::mat4 model = glm::mat4(1.0f);  // Get eye Model matrix
            model = glm::translate(model, modelObjects[i].translation);
            if (modelObjects[i].rotationX != 0.0f) {
                model = glm::rotate(model, glm::radians(modelObjects[i].rotationX), glm::vec3(1, 0, 0));
            }
            if (modelObjects[i].rotationY != 0.0f) {
                model = glm::rotate(model, glm::radians(modelObjects[i].rotationY), glm::vec3(0, 1, 0));
            }
            if (modelObjects[i].rotationZ != 0.0f) {
                model = glm::rotate(model, glm::radians(modelObjects[i].rotationZ), glm::vec3(0, 0, 1));
            }
            model = glm::scale(model, modelObjects[i].scale);
            // Done! 
            glm::mat4 model_mvp = Projection * rotated_view * model;
            glm::mat3 model_normalMat = (glm::mat3)glm::transpose(glm::inverse(model));

            MVPMatID = glGetUniformLocation(programID, "MVP");
            glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &model_mvp[0][0]);
            glUniformMatrix4fv(modelMatId, 1, GL_FALSE, &model[0][0]);
            glUniformMatrix3fv(normalMatID, 1, GL_FALSE, &model_normalMat[0][0]);
            glUniform1i(glGetUniformLocation(programID, "debug_draw_normals"),
                debug_draw_normals);
            glUniform1i(glGetUniformLocation(programID, "debug_draw_texcoords"),
                debug_draw_texcoords);
            glUniform1i(glGetUniformLocation(programID, "debug_disable_lighting"),
                debug_disable_lighting);
            glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
            glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Also set light's color (white)
            modelObjects[i].model.Draw(programID);
        }

		for (int i = 0; i < sizeof(drawObjects) / sizeof(DrawObject); i++) {
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
			glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
           
			if (drawObjects[i].program == programID) {
                glUniformMatrix4fv(modelMatId, 1, GL_FALSE, &model[0][0]);
                glUniformMatrix3fv(normalMatID, 1, GL_FALSE, &normalMat[0][0]);
                glUniform1i(glGetUniformLocation(programID, "debug_draw_normals"),
                    debug_draw_normals);
                glUniform1i(glGetUniformLocation(programID, "debug_draw_texcoords"),
                    debug_draw_texcoords);
                glUniform1i(glGetUniformLocation(programID, "debug_disable_lighting"),
                    debug_disable_lighting);
				glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
				glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Also set light's color (white)
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
			}
			if (drawObjects[i].tex_id != NULL) {
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

        DebugDraw(Projection, rotated_view);

		ImGui_ImplGlfwGL3_NewFrame();
		/*
		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}
		*/
		static bool p_open = true;
		ShowExampleAppConsole(&p_open);
		//static ExampleAppConsole console;
		//console.Draw("Example: Console", p_open);

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