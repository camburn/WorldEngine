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
    float x, y, w, h;
    if (!PyArg_ParseTuple(args, "ffff", &x, &y, &w, &h)) {
        return NULL;
    }
    UpdatePlaneBuffers(vec2(x, y), w, h);
    return Py_BuildValue("");
}

// Create a method list to give to our module
static PyMethodDef PlanesMethods[] = {
    { "new",  pyengine_drawplane, METH_VARARGS, "Draw a plane." },
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