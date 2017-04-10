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
#include "graphics/shader.hpp"
#include "graphics/cube.hpp"
#include "graphics/mesh.hpp"
#include "graphics/console.hpp"
//#include "graphics/text.hpp"

GLFWwindow* window;
int width = 1024;
int height = 768;

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
    PyImport_AppendInittab("emb", &PyInit_emb);
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
    PyRun_SimpleString(
        "import emb\n"
        "print('Result: {}'.format(emb.testfunction(2, 2, 2)))\n"
        "emb.stringfunc('A test c print')\n"
    );
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
    GLuint programID = BuildGlProgram("./src/shaders/vertex_shader.glsl", 
                                      "./src/shaders/fragment_shader.glsl");
	GLuint simple_program = BuildGlProgram("./src/shaders/simple_vertex_shader.glsl",
										   "./src/shaders/simple_fragment_shader.glsl");
    glUseProgram(programID);
	//glEnable(GL_CULL_FACE); // FIX YA NORMALS!
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	glViewport(0, 0, width, height);

	// Mesh Objects
	GLuint CubeMesh = BufferMeshDataVNT(cube_data_normal, sizeof(cube_data_normal));
	GLuint LightMesh = BufferMeshDataVT(cube_data, sizeof(cube_data));

    // Load Textures
    int tex_w, tex_h;
    unsigned char* image = SOIL_load_image("./assets/textures/container.jpg", &tex_w, &tex_h, 0, SOIL_LOAD_RGB);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

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
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		viewPos, // Light Pos
		glm::vec3(0, 0, 0), // Look At
		glm::vec3(0, 1, 0)  // Up
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;


	// Create our Objects
	DrawObject drawObjects[] = {
		{ CubeMesh, texture, glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube1"},
		{ CubeMesh, texture, glm::vec3(2, 1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube2" },
		{ CubeMesh, texture, glm::vec3(1, 2, 2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), programID, "Cube3" },
		{ LightMesh, NULL, lightPos, glm::vec3(0, 0, 0), glm::vec3(0.25, 0.25, 0.25), simple_program, "Light1" }
	};

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

    // Main Loop
    glClearColor(0.0f, 0.25f, 0.25f, 0.0f);
    do {    
		glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (int i = 0; i < sizeof(drawObjects) / sizeof(DrawObject); i++) {
			if (drawObjects[i].name == "Light1") {
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
			glm::mat4 mvp = Projection * View * model;
			glm::mat3 normalMat = (glm::mat3)glm::transpose(glm::inverse(model));
			// Load camera to OpenGL
			glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(modelMatId, 1, GL_FALSE, &model[0][0]);
			glUniformMatrix3fv(normalMatID, 1, GL_FALSE, &normalMat[0][0]);
			if (drawObjects[i].program == programID) {
				glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
				glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Also set light's color (white)
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
			}
			if (drawObjects[i].tex_id != NULL) {
				glBindTexture(GL_TEXTURE_2D, drawObjects[i].tex_id);
			}
			glBindVertexArray(drawObjects[i].mesh_id);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

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

    if (Py_FinalizeEx() < 0) {
        exit(120);
    };
    PyMem_RawFree(program);
    return 0;
}