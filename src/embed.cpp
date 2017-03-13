/*
This is an example C application that has an embedded python interpreter.
It provides C function calls that be called from Python and
Python function calls that can be called from C.
*/
#include <Python.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "python_api.hpp"
#include "tools/shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLFWwindow* window;
int width = 1024;
int height = 768;

int main(int argc, char *argv[]) {
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

    // GL stuff
    GLuint programID = BuildGlProgram("./src/shaders/vertex_shader.glsl", 
                                      "./src/shaders/fragment_shader.glsl");
    glUseProgram(programID);

    // Set up Cameras
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, 3),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Create our VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create our Triangles data
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    // Buffer data to VBO
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    glClearColor(0.0f, 0.25f, 0.25f, 0.0f);
    do {
        // Main Loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Load camera to OpenGL
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        // Drawing tasks
        glBindVertexArray(VertexArrayID);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    glfwTerminate();
    // END OPENGL STUFF

    if (Py_FinalizeEx() < 0) {
        exit(120);
    };
    PyMem_RawFree(program);
    return 0;
}