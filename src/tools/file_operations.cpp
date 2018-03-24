#include "tools/file_operations.hpp"

PyObject *pModule;

void unload_shaders() {
    Py_DECREF(pModule);
}

char* loadFile(const char* filepath) {
    /*
    Wrapper around a python call.
    There must be a nice way to make this generic, will think about this later.
    TODO - This doesn't belong in our shader tools
    */
    PyObject *pName, *pFunc;
    PyObject *pArgs, *pValue;
    char* contents;

    if (!pModule) {
        pName = PyUnicode_FromString("console.utils");
        pModule = PyImport_Import(pName);
    }

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

    Py_DECREF(pFunc);
    return contents;
}


