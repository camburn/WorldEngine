#include "python_manager_api.hpp"

// State Manager
class Storage {
public:
    Storage(State &state, PrimitiveManager &primitive, MeshManager &mesh) 
        : state_manager(state), primitive_manager(primitive), mesh_manager(mesh) {}
    State &state_manager;
    PrimitiveManager &primitive_manager;
    MeshManager &mesh_manager;
};

Storage *storage;

void assign_managers(State &state, PrimitiveManager &primitive, MeshManager &mesh) {
    storage = new Storage(state, primitive, mesh);
}

static PyObject*
pyengine_set_mesh_rotation(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->mesh_manager.update_instance_rotation(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyObject*
pyengine_set_mesh_position(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->mesh_manager.update_instance_position(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyObject*
pyengine_set_mesh_scale(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->mesh_manager.update_instance_scale(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyObject*
pyengine_create_primitive(PyObject *self, PyObject *args) {
    char* primitive_type;
    char* texture_name;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ssfff", &primitive_type, &texture_name, &x, &y, &z)) {
        return NULL;
    }
    std::string str_primitive_type(primitive_type);
    std::string str_texture_name(texture_name);
    //("Cube", "wooden_crate", glm::vec3(-2, 0.5, 0));
    unsigned int instance_id = storage->primitive_manager.new_instance(
        str_primitive_type, 
        str_texture_name, 
        glm::vec3(x, y, z)
    );
    return Py_BuildValue("i", instance_id);
}

static PyObject*
pyengine_set_primitive_rotation(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->primitive_manager.update_instance_rotation(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyObject*
pyengine_set_primitive_position(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->primitive_manager.update_instance_position(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyObject*
pyengine_set_primitive_scale(PyObject *self, PyObject *args) {
    int index;
    float x, y, z;
    if (!PyArg_ParseTuple(args, "ifff", &index, &x, &y, &z)) {
        return NULL;
    }
    storage->primitive_manager.update_instance_scale(index, glm::vec3(x, y, z));
    return Py_BuildValue("");
}

static PyMethodDef engine_manager_methods[] = {
    { "create_primitive",  pyengine_create_primitive, METH_VARARGS, "Create a new primitive." },
    { "set_primitive_rotation",  pyengine_set_primitive_rotation, METH_VARARGS, "Set the rotation of a primitive." },
    { "set_primitive_position",  pyengine_set_primitive_position, METH_VARARGS, "Set the position of a primitive." },
    { "set_primitive_scale",  pyengine_set_primitive_scale, METH_VARARGS, "Set the scale of a primitive." },
    { "set_mesh_rotation",  pyengine_set_mesh_rotation, METH_VARARGS, "Set the rotation of a mesh." },
    { "set_mesh_position",  pyengine_set_mesh_position, METH_VARARGS, "Set the position of a mesh." },
    { "set_mesh_scale",  pyengine_set_mesh_scale, METH_VARARGS, "Set the scale of a mesh." },
    {NULL, NULL, 0, NULL}
};

// Create a python module
static PyModuleDef engine_manager_module = {
    PyModuleDef_HEAD_INIT, "manager", NULL, -1, engine_manager_methods,
    NULL, NULL, NULL, NULL
};

PyObject* 
PyInit_manager(void) {
    return PyModule_Create(&engine_manager_module);
}