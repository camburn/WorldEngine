#include "engine.hpp"
#include "python_api.hpp"

#include "input.hpp"
#include "Engine/physics/raycast.hpp"
#include "Engine/renderer/camera.hpp"

namespace engine {

// ===== Input Module =====
static PyObject* py_get_mouse_position(PyObject *self, PyObject *args) {
    std::pair<float, float> result = get_mouse_position();
    return Py_BuildValue("ff", result.first, result.second);
}

static PyObject* py_mouse_intersect_plane(PyObject *self, PyObject *args) {
    float p_x, p_y, p_z, n_x, n_y, n_z;
    if (!PyArg_ParseTuple(args, "ffffff", &p_x, &p_y, &p_z, &n_x, &n_y, &n_z)) {
        return NULL;
    }

    glm::vec3 plane_point {p_x, p_y, p_z};
    glm::vec3 plane_normal {n_x, n_y, n_z};

    std::pair<float, float> result = get_mouse_position();
    auto camera = get_main_camera();
    Ray ray = cast_ray(camera);
    glm::vec3 point = intersect_ray_plane(ray, plane_point, plane_normal);

    return Py_BuildValue("fff", point.x, point.y, point.z);
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
    {"mouse_intersect_plane", py_mouse_intersect_plane, METH_VARARGS, "Get 3d position to where mouse intersects plane."},
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