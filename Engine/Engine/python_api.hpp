#ifndef _PYTHON_API_HPP
#define _PYTHON_API_HPP

#include <Python.h>
#include <structmember.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Engine/scripts.hpp"

#ifdef ENGINE_PLATFORM_LINUX
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#ifdef ENGINE_PLATFORM_WINDOWS
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace engine {

PyMODINIT_FUNC PyInit_py_script(void);

void script_init();

PyObject *new_script();


class PythonScript: public Script{
public:
    PythonScript(std::string name, std::shared_ptr<Object> parent);

    ~PythonScript();

    virtual void reload() override;

    virtual void update(float delta_time) override;
    virtual void on_click(std::shared_ptr<Event> event) override;

    PyObject_HEAD
    PyObject *py_name;
    PyObject *py_script;
    PyObject *module_name;
    PyObject *script_module;
    PyObject *update_func;
    PyObject *on_click_func;
    std::string module_path;
    fs::file_time_type last_modify;

private:
    bool script_failed = false;
    friend PyObject* py_script_set_enabled(PythonScript* self, PyObject* args, PyObject* kwargs);
    friend PyObject* py_script_get_enabled(PythonScript* self, PyObject* Py_UNUSED(ignored));
    friend PyObject* py_script_set_transform(PythonScript *self, PyObject *args, PyObject *kwargs);
    friend PyObject* py_script_get_transform(PythonScript *self, PyObject *Py_UNUSED(ignored));
    friend PyObject* py_script_get_rotation(PythonScript *self, void *closure);
    friend int py_script_set_rotation(PythonScript *self, PyObject *value, void *closure);
    friend PyObject* py_script_get_name(PythonScript *self, void *closure);
    friend int py_script_set_name(PythonScript *self, PyObject *value, void *closure);
};


} // End namespace

#endif