#include "engine.hpp"
#include "python_api.hpp"

namespace engine {

static const std::string DEFAULT_MODULE = R"(""" Template Python Script """
from scripts.engine import engine_call

@engine_call
def update(obj, delta_time):
    """ Update is called every frame. """

@engine_call
def on_click(obj):
    """ on_click is called when an object is clicked on with a collider """

)";

bool handle_exception(std::string name) {
    if (PyErr_Occurred()) {
        PyObject *type, *value, *traceback;

        PyErr_Fetch(&type, &value, &traceback);
        PyErr_NormalizeException(&type, &value, &traceback);

        PyObject* str_exc_type = PyObject_Repr(type);
        PyObject* pystr = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");
        std::string str_type =  std::string(PyBytes_AS_STRING(pystr));

        PyObject* str_exc_value = PyObject_Repr(value);
        PyObject* excstr = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "Error ~");
        std::string str_value =  std::string(PyBytes_AS_STRING(excstr));

        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);

        Py_XDECREF(str_exc_type);
        Py_XDECREF(pystr);

        Py_XDECREF(excstr);
        Py_XDECREF(str_exc_value);

        if (str_type == name) {
            return true;
        }

        ENGINE_ERROR("Unhandled Python Exception: {0}", str_value);
        return false;
    }
    ENGINE_INFO("No Exception: {0}");
    return false;
}

PythonScript::PythonScript(std::string name, std::shared_ptr<Object> parent): Script(name, parent) {
    module_name = PyUnicode_FromString(("scripts." + name).c_str());
    script_module = PyImport_Import(module_name);
    if (script_module == NULL) {
        if (handle_exception("<class 'ModuleNotFoundError'>")) {
            ENGINE_INFO("Python script does not exist, creating base file - {0}", name);
            std::ofstream out( "./scripts/" + name + ".py" );
            out << DEFAULT_MODULE;
            out.close();
            module_name = PyUnicode_FromString(("scripts." + name).c_str());
            script_module = PyImport_Import(module_name);
            if (script_module == NULL) {
                ENGINE_ASSERT(script_module, "Failed to set up script");
                script_failed = true;
                return;
            }
        } else {
            ENGINE_ERROR("Script {0} failed", name);
            script_failed = true;
            return;
        }
    }

    PyObject *p_module_path = PyObject_GetAttrString(script_module, "__file__");
    module_path = PyUnicode_AsUTF8(p_module_path);
    last_modify = fs::last_write_time(module_path);

    // Read file into memory
    std::ifstream t(module_path);
    source = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    update_func = PyObject_GetAttrString(script_module, "update");
    if (update_func == NULL) {
        ENGINE_ERROR("Python script module not found - {0}", name);
    }
    if (PyObject_HasAttrString(script_module, "on_click")) {
        on_click_func = PyObject_GetAttrString(script_module, "on_click");
        if (on_click_func != NULL) {
            bus::register_callback(
                std::bind(&PythonScript::on_click, this, std::placeholders::_1),
                ENGINE_PHYSICS_RAYCAST_HIT
            );
        }
    }
    // Create the python script instance
    py_script = new_script();
    // Python c api uses the memory layout of this class.
    // That is why it is safe for us to perform the following
    ((PythonScript*)py_script)->parent = parent;
}

PythonScript::~PythonScript() {
    // TODO: Deallocate memory and deallocate python references to objects
    // Py_DECREF(py_name);
    Py_DECREF(py_script);
    Py_DECREF(module_name);
    Py_DECREF(script_module);
    Py_DECREF(update_func);
}

void PythonScript::reload() {
    std::ofstream out(module_path);
    source.erase(std::find(source.begin(), source.end(), '\0'), source.end());
    source += '\n';
    out << source;
    out.close();
}

void PythonScript::update(float delta_time) {
    if (script_failed) return;

    #ifdef ENGINE_DEBUG_ENABLED
    // Check if the script has been updated
    if (last_modify < fs::last_write_time(module_path)) {
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
        last_modify = fs::last_write_time(module_path);
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

void PythonScript::on_click(std::shared_ptr<Event> event) {
    if (auto ray_event = std::dynamic_pointer_cast<RaycastHitEvent>(event)) {
        // FIXME: This check smells
        if (parent->attached(Object::COLLIDER) &&
                &ray_event->hit.hit_target == parent->collider().get()) {
            PyObject* result = PyObject_CallFunction(on_click_func, "O", py_script);
        }
    }
}

static void py_script_dealloc(PythonScript* self) {
    //Py_XDECREF(self->py_name);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject* py_script_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PythonScript* self;

    self = (PythonScript*) type->tp_alloc(type, 0);
    if (self != NULL) {
        
        self->py_name = PyUnicode_FromString("");
    }
    return (PyObject *) self;
}

static int py_script_init(PythonScript* self, PyObject *args, PyObject *kwargs) {
    return 0;
}

// Pure members are not particulary compatible with c++ classes, use
// getter/setters instead
static PyMemberDef py_script_members[] = {
    //{"name", T_OBJECT_EX, offsetof(PythonScript, py_name), 0, "Name"},
    //{"number", T_INT, offsetof(Script, number), 0, "custom number"},
    {NULL}  // Sentinel
};

// METHODS

PyObject* py_script_set_transform(PythonScript* self, PyObject* args, PyObject* kwargs) {
    PyObject *name;
    float x, y, z;
    static char *kwlist[] = {(char*)"x", (char*)"y", (char*)"z", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", kwlist, &x, &y, &z) ) {
        return NULL;
    }
    self->parent->transform().set_translation(glm::vec3(x, y, z));
    return Py_BuildValue("");
}

PyObject* py_script_get_transform(PythonScript *self, PyObject *Py_UNUSED(ignored)) {
    glm::vec3 t = self->parent->transform().get_translation();

    return Py_BuildValue("fff", t.x, t.y, t.z);
}

static PyMethodDef py_script_methods[] = {
    {"update_transform", (PyCFunction) py_script_set_transform, METH_VARARGS | METH_KEYWORDS, "Multiply args."},
    {"get_transform", (PyCFunction) py_script_get_transform, METH_NOARGS, "Multiply args."},
    //{"update_object_rotation", engine_updateobjectrotation, METH_VARARGS | METH_KEYWORDS, "Multiply args."},
    {NULL}  // Sentinel
};

// GETTERS AND SETTERS (properties)
PyObject* py_script_get_rotation(PythonScript *self, void *closure) {
    glm::quat q = self->parent->transform().get_rotation();
    return Py_BuildValue("ffff", q.x, q.y, q.z, q.w);
}

int py_script_set_rotation(PythonScript *self, PyObject *value, void *closure) {
    float x, y, z;
    if (!PyArg_ParseTuple(value, "fff", &x, &y, &z)) {
        return -1;
    }

    glm::quat pitch = glm::angleAxis(x, glm::vec3(1, 0, 0));
    glm::quat yaw = glm::angleAxis(y, glm::vec3(0, 1, 0));
    glm::quat roll = glm::angleAxis(z, glm::vec3(0, 0, 1));

    glm::quat rotation = yaw * pitch * roll;

    self->parent->transform().set_rotation(rotation);

    return 0;
}

PyObject* py_script_get_name(PythonScript *self, void *closure) {
    if (self->py_name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "name");
        return NULL;
    }
    return PyUnicode_FromFormat("%S", self->py_name);
}

int py_script_set_name(PythonScript *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
            "Set requires 1 argument");
        return -1;
    }
    if (! PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
            "The first attribute value must be a str");
        return -1;
    }

    Py_DECREF(self->py_name);
    Py_INCREF(value);
    self->py_name = value;

    //Py_RETURN_NONE;
    return 0;
}

static PyGetSetDef py_script_getseters[] = {
    {"rotation", (getter)py_script_get_rotation, (setter)py_script_set_rotation, "Object rotation", NULL},
    {"name", (getter)py_script_get_name, (setter)py_script_set_name, "Script Name", NULL},
    {NULL}  // Sentinel
};

static PyTypeObject PyScriptType {
    PyVarObject_HEAD_INIT(NULL, 0)
};

static PyModuleDef engine_module {
    PyModuleDef_HEAD_INIT
};

// c++ struct initialization is a bit of a mess between compilers
// on the MSVC compiler you cannot mix named attributes (.tp_name)
// with header macros while this is supported on Linux.
// In order to protect against the PyTypeObject struct changes between
// Python versions we are using the following function to initialize it.
void init_type() {
    PyScriptType.tp_name = "engine.Script",
    PyScriptType.tp_basicsize = sizeof(Script);
    PyScriptType.tp_dealloc = (destructor) py_script_dealloc;
    PyScriptType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyScriptType.tp_doc = "Engine PythonScriptObject";
    PyScriptType.tp_methods = py_script_methods;
    PyScriptType.tp_members = py_script_members;
    PyScriptType.tp_getset = py_script_getseters;
    PyScriptType.tp_init = (initproc)py_script_init;
    PyScriptType.tp_new = py_script_new;

    engine_module.m_name = "engine";
    engine_module.m_doc = "Engine module for engine API.";
    engine_module.m_size = -1;
}


PyMODINIT_FUNC PyInit_py_script(void) {
    PyObject *m;
    if (PyType_Ready(&PyScriptType) < 0) {
        ENGINE_ASSERT(0,
            "Error validating PyScriptType - This is probly caused"
            "by a PyTypeObject struct change (check python version)"
        );
    }
    m = PyModule_Create(&engine_module);
    if (m == NULL) {
        ENGINE_ASSERT(0, "Error creating PyScriptType module");
    }

    Py_INCREF(&PyScriptType);
    if (PyModule_AddObject(m, "Script", (PyObject *) &PyScriptType) < 0) {
        Py_DECREF(&PyScriptType);
        Py_DECREF(m);
        ENGINE_ASSERT(0, "Failed to initialise Python - Object instance creation failed");
    }

    return m;
}

PyObject *script_class;

void script_init() {
    if (Py_IsInitialized()) return;
    //Py_SetProgramName("EngineTest");
    //Py_SetPath(L"./"
    //PyImport_AppendInittab("engine_static", &PyInit_engine_static);
    ENGINE_INFO("Initialising Python scripting");
    init_type();
    PyImport_AppendInittab("engine", &PyInit_py_script);
    Py_Initialize();

    PyRun_SimpleString(
        "import sys\n"
        "sys.path.append('./')\n"
    );
    script_class = PyObject_GetAttrString(PyInit_py_script(), "Script");
    ENGINE_ASSERT(script_class, "Object was not made.");
}

PyObject *new_script() {
    ENGINE_ASSERT(Py_IsInitialized(), "Python is not initialised, cannot create PythonScript - call `script_init()`");
    PyObject *args = Py_BuildValue("(s)", "Python Script Instance");
    PyObject *script_instance = PyObject_CallObject(script_class, args);
    PyErr_Print();
    return script_instance;
}


} // namespace