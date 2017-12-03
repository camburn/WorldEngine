#include "python_api.hpp"

/* BEGIN - C API to present to our Python instance */
// Methods
/* Python Debug Tools Interface */
// Python needs to be able to call CreateLine
static PyObject*
pyengine_debug_drawline(PyObject *self, PyObject *args) {
    float x1, y1, z1, x2, y2, z2, r, g, b;
    if (!PyArg_ParseTuple(args, "fffffffff", &x1, &y1, &z1, &x2, &y2, &z2, &r, &g, &b)) {
        return NULL;
    }
    DebugCreateLine(x1, y1, z1, x2, y2, z2, r, g, b);
    return Py_BuildValue("");
}

static PyObject*
pyengine_debug_drawcube(PyObject *self, PyObject *args) {
    float x, y, z, w, h, d, r, g, b;
    if (!PyArg_ParseTuple(args, "fffffffff", &x, &y, &z, &w, &h, &d, &r, &g, &b)) {
        return NULL;
    }
    DebugCreateCube(x, y, z, w, h, d, r, g, b);
    return Py_BuildValue("");
}

static PyObject*
pyengine_debug_clear(PyObject *self, PyObject *args) {
    DebugClear();
    return Py_BuildValue("");
}

static PyObject*
pyengine_debug_flatset(PyObject *self, PyObject *args) {
    char* str_data;
    if (!PyArg_ParseTuple(args, "s", &str_data)) {
        return NULL;
    }
    std::string flagList = DebugFlagToggle(str_data);
    return Py_BuildValue("s", flagList.c_str());
}

static PyObject*
pyengine_debug_flatlist(PyObject *self, PyObject *args) {
    std::string flagList = DebugFlagList();
    return Py_BuildValue("s", flagList.c_str());
}

/* Example Items */
static PyObject*
pyengine_testfunction(PyObject *self, PyObject *args) {
    double a, b, c;
    if (!PyArg_ParseTuple(args, "ddd", &a, &b, &c)) {
        return NULL;
    }
    return Py_BuildValue("d", a * b * c);
}

static PyObject*
pyengine_stringfunc(PyObject *self, PyObject *args) {
    char* str_data;
    if (!PyArg_ParseTuple(args, "s", &str_data)) {
        return NULL;
    }
    fprintf(stdout, "COUT: %s\n", str_data);
    return Py_BuildValue("");
}

// Create a method list to give to our module
static PyMethodDef DebugMethods[] = {
    { "drawline", pyengine_debug_drawline, METH_VARARGS, "Draw a debug line." },
    { "drawcube", pyengine_debug_drawcube, METH_VARARGS, "Draw a debug cube." },
    { "clear", pyengine_debug_clear, METH_VARARGS, "Clear the debug layers." },
    { "flag_list", pyengine_debug_flatlist, METH_VARARGS, "List all engine debug flags." },
    { "flag_set", pyengine_debug_flatset, METH_VARARGS, "Set an engine debug flag." },
    { "testfunction", pyengine_testfunction, METH_VARARGS, "Multiply args."},
    { "stringfunc", pyengine_stringfunc, METH_VARARGS, "C print a python str."},
    {NULL, NULL, 0, NULL}
};

// Create a python module
static PyModuleDef DebugModule = {
    PyModuleDef_HEAD_INIT, "debug", NULL, -1, DebugMethods,
    NULL, NULL, NULL, NULL
};

PyObject* 
PyInit_Debug(void) {
    return PyModule_Create(&DebugModule);
}
/* END - C API */

/* BEGIN 2D LIBRARY - C API */
static PyObject*
pyengine_drawplane(PyObject *self, PyObject *args) {
    char* str_data;
    float x, y, w, h;
    int tex_id;
    if (!PyArg_ParseTuple(args, "sffffi", &str_data, &x, &y, &w, &h, &tex_id)) {
        return NULL;
    }
    UpdatePlaneBuffers(w, h, str_data, vec2(x, y), tex_id);
    return Py_BuildValue("");
}

static PyObject*
pyengine_planematrix(PyObject *self, PyObject *args) {
    char* str_data;
    float x0, y0, z0, w0;
    float x1, y1, z1, w1;
    float x2, y2, z2, w2;
    float x3, y3, z3, w3;
    if (!PyArg_ParseTuple(args, "sffffffffffffffff", &str_data, 
        &x0, &y0, &z0, &w0,
        &x1, &y1, &z1, &w1,
        &x2, &y2, &z2, &w2,
        &x3, &y3, &z3, &w3)) {
        return NULL;
    }
    glm::mat4 model_mat = mat4(
        x0, y0, z0, w0, 
        x1, y1, z1, w1, 
        x2, y2, z2, w2, 
        x3, y3, z3, w3
    );
    UpdatePlaneMatrix(str_data, model_mat);
    return Py_BuildValue("");
}

static PyObject*
pyengine_eyeplanematrix(PyObject *self, PyObject *args) {
    char* str_data;
    if (!PyArg_ParseTuple(args, "s", &str_data)) {
        return NULL;
    }
    glm::mat4 model_mat = mat4(1.0f);
    UpdatePlaneMatrix(str_data, model_mat);
    return Py_BuildValue("");
}

static PyObject*
pyengine_translate_matrix(PyObject *self, PyObject *args) {
    char* str_data;
    float x0, y0, z0;
    if (!PyArg_ParseTuple(args, "sfff", &str_data, &x0, &y0, &z0)) {
        return NULL;
    }
    TranslatePlaneMatrix(str_data, x0, y0, z0);
    return Py_BuildValue("");
}


/* END 2D LIBRARY - C API */

// Create a method list to give to our module
static PyMethodDef PlanesMethods[] = {
    { "new",  pyengine_drawplane, METH_VARARGS, "Draw a plane." },
    { "matrix",  pyengine_planematrix, METH_VARARGS, "Create a new model matrix." },
    { "eye_matrix",  pyengine_eyeplanematrix, METH_VARARGS, "Create a new eye model matrix." },
    { "translate",  pyengine_translate_matrix, METH_VARARGS, "Create a new eye model matrix." },
    {NULL, NULL, 0, NULL}
};

// Create a python module
static PyModuleDef PlanesModule = {
    PyModuleDef_HEAD_INIT, "planes", NULL, -1, PlanesMethods,
    NULL, NULL, NULL, NULL
};

PyObject* 
PyInit_Planes(void) {
    return PyModule_Create(&PlanesModule);
}
/* END 2D LIBRARY - C API */