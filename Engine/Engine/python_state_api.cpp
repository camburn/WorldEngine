#include "engine.hpp"
#include "python_api.hpp"

#include "input.hpp"

namespace engine {

// ===== Input Module =====
static PyObject* py_get_mouse_position(PyObject *self, PyObject *args) {
    std::pair<float, float> result = get_mouse_position();
    return Py_BuildValue("ff", result.first, result.second);
}

static PyObject* py_is_mouse_clicked(PyObject *self, PyObject *args) {
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    bool result = is_mouse_clicked(a);
    return Py_BuildValue("O", result ? Py_True: Py_False);
}

static PyObject* py_is_key_pressed(PyObject *self, PyObject *args) {
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    bool result = is_key_pressed(a);
    return Py_BuildValue("O", result ? Py_True: Py_False);
}

static PyObject* py_is_key_down(PyObject *self, PyObject *args) {
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    bool result = is_key_down(a);
    return Py_BuildValue("O", result ? Py_True: Py_False);
}

static PyMethodDef input_methods[] = {
    {"is_mouse_clicked", py_is_mouse_clicked, METH_VARARGS, "Get state of mouse button."},
    {"is_key_pressed", py_is_key_pressed, METH_VARARGS, "Get state of key."},
    {"is_key_down", py_is_key_down, METH_VARARGS, "Check if key is down."},
    {"get_mouse_position", py_get_mouse_position, METH_VARARGS, "Get mouse position."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef input_module {
    PyModuleDef_HEAD_INIT,
    "engine_input", NULL, -1, input_methods,
    NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_input(void) {
    PyObject *m = PyModule_Create(&input_module);
    if (m == NULL) {
        ENGINE_ASSERT(0, "Error creating Python Input module");
    }
    return m;
}
// ===== End Input Module =====

} // namespace