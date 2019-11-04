#ifndef _SCRIPTS_HPP
#define _SCRIPTS_HPP

#include <memory>
#include <string>
#include <chrono>

#include <Python.h>

#include <glm/glm.hpp>

#include "Engine/objects.hpp"


namespace engine {

/*
A script should have code that is run every update, sim update, ui update, or 
phyics update.

It should also contain a series of functions to manipulate state of Objects and
their attached components.

Finally they should have a series of defined events, these events can trigger
at the beggining of every of the above update methods but will receive
particular arguments.

i.e. Collision event args(self, other_object) would be called during the physics
update.
*/

static std::shared_ptr<Object> current_object {nullptr};

static PyObject*
engine_updateobjectrotation(PyObject *self, PyObject *args) {
    float x, y, z;
    if (!PyArg_ParseTuple(args, "fff", &x, &y, &z)) {
        return NULL;
    }
    ENGINE_INFO("{0} - Updating rotation: {1},{2},{3}", current_object->name, x, y, z);

    glm::quat pitch = glm::angleAxis(x, glm::vec3(1, 0, 0));
    glm::quat yaw = glm::angleAxis(y, glm::vec3(0, 1, 0));
    glm::quat roll = glm::angleAxis(z, glm::vec3(0, 0, 1));

    glm::quat rotation = yaw * pitch * roll;

    current_object->transform().set_rotation(rotation);

    return Py_BuildValue("");
}

static PyObject*
engine_updateobjecttransform(PyObject *self, PyObject *args) {
    float x, y, z;
    if (!PyArg_ParseTuple(args, "fff", &x, &y, &z)) {
        return NULL;
    }
    ENGINE_INFO("{0} - Updating transform: {1},{2},{3}", current_object->name, x, y, z);

    current_object->transform().set_translation(glm::vec3(x, y, z));

    return Py_BuildValue("");
}

static PyObject*
engine_getobjecttransform(PyObject *self, PyObject *args) {
    //float x, y, z;
    //if (!PyArg_ParseTuple(args, "fff", &x, &y, &z)) {
    //    return NULL;
    //}
    //ENGINE_INFO("{0} - Updating transform: {1},{2},{3}", current_object->name, x, y, z);

    glm::vec3 t = current_object->transform().get_translation();

    return Py_BuildValue("fff", t.x, t.y, t.z);
}

// Create a method list to give to our module
static PyMethodDef EngineMethods[] = {
    {"update_object_transform", engine_updateobjecttransform, METH_VARARGS, "Multiply args."},
    {"get_object_transform", engine_getobjecttransform, METH_VARARGS, "Multiply args."},
    {"update_object_rotation", engine_updateobjectrotation, METH_VARARGS, "Multiply args."},
    {NULL, NULL, 0, NULL}
};

// Create a python module
static PyModuleDef EngineModule = {
    PyModuleDef_HEAD_INIT, "engine", NULL, -1, EngineMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_engine(void) {
    return PyModule_Create(&EngineModule);
}

class Script {

public:
    Script(std::string name, std::shared_ptr<Object> parent): name(name), parent(parent) {
        
        //Py_SetProgramName("EngineTest");
        //
        //Py_SetPath(L"./");
        PyImport_AppendInittab("engine", &PyInit_engine);
        Py_Initialize();
        
        PyRun_SimpleString(
            "import sys\n"
            "sys.path.append('./')\n"
        );

        PyObject *pName = PyUnicode_FromString("scripts.ball_spin");
        pModule = PyImport_Import(pName);
        if (pModule != NULL) {
            update_function = PyObject_GetAttrString(pModule, "update");
            if (update_function && PyCallable_Check(update_function)) {
                ENGINE_INFO("Update function found");
            }

        } else {
            ENGINE_ERROR("Cannot import module.");
        }
        Py_DECREF(pName);
    }

    ~Script(){
        Py_DECREF(pModule);
        Py_DECREF(update_function);

        if (Py_FinalizeEx() < 0)
            exit(120);
    }

    void update(float delta_time) {
        using milli = std::chrono::milliseconds;
        auto start = std::chrono::high_resolution_clock::now();
    
        //last_script = this;
        current_object = parent;
        if (update_function && PyCallable_Check(update_function)) {
                    PyObject *pArgs, *pValue;
            pArgs = PyTuple_Pack(1, Py_BuildValue("f", delta_time));
            
            pValue = PyObject_CallObject(update_function, pArgs);
            // TODO: Handle response?
            if (pValue) {
                Py_DECREF(pValue);
            } else {
                PyErr_PrintEx(1);
            }
            Py_DECREF(pArgs);

            auto finish = std::chrono::high_resolution_clock::now();
            ENGINE_INFO("Update Took: {0} milliseconds", std::chrono::duration_cast<milli>(finish - start).count());
            return;
        }
        ENGINE_WARN("Update function not initialised");
    }
    std::string name;

private:
    std::string script;
    std::shared_ptr<Object> parent;

    PyObject *pModule;
    PyObject *update_function;

};

}

#endif