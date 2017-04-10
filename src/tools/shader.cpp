#include "graphics/shader.hpp"

char* loadFile(const char* filepath) {
    /*
    Wrapper around a python call.
    There must be a nice way to make this generic, will think about this later.
    TODO - This doesn't belong in our shader tools
    */
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    char* contents;

    pName = PyUnicode_FromString("console.utils");
    pModule = PyImport_Import(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "load_file");
        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_Pack(1, Py_BuildValue("s", filepath));
            pValue = PyObject_CallObject(pFunc, pArgs);
            // This converts our python return object to something C understands
            contents = PyBytes_AsString(PyUnicode_AsASCIIString(pValue));
            Py_DECREF(pArgs);
            Py_DECREF(pValue);
        } else {
            PyErr_Print();
            fprintf(stderr, "Failed to find function - %s", "load_file");
            return NULL;            
        }
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load module - %s", "console.utils");
        return NULL;
    }
    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    return contents;
}

GLuint LoadShader(const char* file_path, GLuint ShaderID) {   
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char* vertex_src = loadFile(file_path);
    fprintf(stdout, "Got Shader Source\n\n%s\n\n", vertex_src);
    // Compile the Shader
    fprintf(stdout, "Compiling Shader: %s\n", file_path);
    glShaderSource(ShaderID, 1, &vertex_src, NULL);
    glCompileShader(ShaderID);
    // Check result
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ShaderErrorMessage = (char *)malloc(InfoLogLength);
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        fprintf(stderr, "Shader Compile Failed:\n %s\n", ShaderErrorMessage);
        free(ShaderErrorMessage);
    }
    return ShaderID;
}

GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path) {
    GLuint vertexShaderID = LoadShader(vertex_file_path, glCreateShader(GL_VERTEX_SHADER));
    GLuint fragmentShaderID = LoadShader(fragment_file_path, glCreateShader(GL_FRAGMENT_SHADER));
    GLuint programID = glCreateProgram();
    GLint Result = GL_FALSE;
    int InfoLogLength;
    fprintf(stdout, "Linking shaders to program\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ProgramErrorMessage = (char *)malloc(InfoLogLength);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "Program Link Failure:\n %s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
    }
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}