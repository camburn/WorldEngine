#ifndef _PYTHON_API_HPP
#define _PYTHON_API_HPP

#include <Python.h>
#include <structmember.h>
#include <experimental/filesystem>

#include "Engine/scripts.hpp"

namespace engine {

PyMODINIT_FUNC PyInit_py_script(void);

void script_init();

PyObject *new_script();


class PythonScript: public Script{
public:
    PythonScript(std::string name, std::shared_ptr<Object> parent): Script(name, parent){
        module_name = PyUnicode_FromString(name.c_str());
        script_module = PyImport_Import(module_name);
        if (script_module == NULL) { 
            ENGINE_ERROR("Python script module not found - {0}", name);
        }

        PyObject *p_module_path = PyObject_GetAttrString(script_module, "__file__");
        module_path = PyUnicode_AsUTF8(p_module_path);
        last_modify = std::experimental::filesystem::last_write_time(module_path);

        update_func = PyObject_GetAttrString(script_module, "update");
        if (update_func == NULL) { 
            ENGINE_ERROR("Python script module not found - {0}", name);
        }
        // Create the python script instance
        py_script = new_script();
        // Python c api uses the memory layout of this class.
        // That is why it is safe for us to perform the following
        ((PythonScript*)py_script)->parent = parent;
    }

    ~PythonScript() {
        // TODO: Deallocate memory and deallocate python references to objects
        Py_DECREF(py_name);
        Py_DECREF(py_script);
        Py_DECREF(module_name);
        Py_DECREF(script_module);
        Py_DECREF(update_func);
    }

    virtual void update(float delta_time) override {
        #ifdef ENGINE_DEBUG_ENABLED
        // Check if the script has been updated
        if (last_modify < std::experimental::filesystem::last_write_time(module_path)) {
            ENGINE_INFO("Script reloaded");
            PyObject* new_module = PyImport_ReloadModule(script_module);
            if (new_module == NULL) { 
                ENGINE_ERROR("Python module could not be reloaded - {0}", name);
                if (PyErr_Occurred()) {
                    ENGINE_ERROR("Error in script");
                    PyErr_PrintEx(1);
                }
            } else {
                Py_DECREF(script_module);
                script_module = new_module;
                update_func = PyObject_GetAttrString(script_module, "update");
            }
            last_modify = std::experimental::filesystem::last_write_time(module_path);
        }
        #endif
        // Call the python update with our object
        PyObject* result = PyObject_CallFunction(update_func, "Of", py_script, delta_time);
        if (PyErr_Occurred()) {
            ENGINE_ERROR("Error in script");
            PyErr_PrintEx(1);
        }
        Py_XDECREF(result);
    }

    PyObject_HEAD
    PyObject *py_name;
    PyObject *py_script;
    PyObject *module_name;
    PyObject *script_module;
    PyObject *update_func;
    std::string module_path;
    std::experimental::filesystem::file_time_type last_modify;

private:
    friend PyObject* py_script_set_transform(PythonScript *self, PyObject *args, PyObject *kwargs);
    friend PyObject* py_script_get_transform(PythonScript *self, PyObject *Py_UNUSED(ignored));
    friend PyObject* py_script_get_rotation(PythonScript *self, void *closure);
    friend int py_script_set_rotation(PythonScript *self, PyObject *value, void *closure);
};


} // End namespace

#endif