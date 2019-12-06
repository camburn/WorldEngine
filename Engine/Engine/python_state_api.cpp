#include "engine.hpp"
#include "python_api.hpp"

#include "input.hpp"

namespace engine {

static PyObject* get_mouse_button(PyObject *self, PyObject *args) {
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    bool result = get_mouse_button(a);
    return Py_BuildValue("O", result ? Py_True: Py_False);
}

static PyMethodDef input_methods[] = {
    {"get_mouse_button", get_mouse_button, METH_VARARGS, "Get state of mouse button."},
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

} // namespace