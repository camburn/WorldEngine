#include "engine.hpp"
#include "python_api.hpp"

namespace engine {


static void py_script_dealloc(PythonScript* self) {
    Py_XDECREF(self->py_name);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject* py_script_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PythonScript* self;

    self = (PythonScript*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->py_name = PyUnicode_FromString("");
        if (self->py_name == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }
    return (PyObject *) self;
}

static int py_script_init(PythonScript* self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {(char*)"name", NULL};
    PyObject *name = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "|O", kwlist, &name)
        )
        return -1;

    if (name) {
        tmp = self->py_name;
        Py_INCREF(name);
        self->py_name = name;
        Py_XDECREF(tmp);
    }
    return 0;
}

static PyMemberDef py_script_members[] = {
    {"name", T_OBJECT_EX, offsetof(PythonScript, py_name), 0, "Name"},
    //{"number", T_INT, offsetof(Script, number), 0, "custom number"},
    {NULL}  /* Sentinel */
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

static PyObject* py_script_get_name(PythonScript *self, PyObject *Py_UNUSED(ignored)) {
    if (self->py_name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "name");
        return NULL;
    }
    return PyUnicode_FromFormat("%S", self->py_name);
}

static PyObject *py_script_set_name(PythonScript *self, PyObject *args, PyObject *kwargs) {
    PyObject *name;
    static char *kwlist[] = {(char*)"name", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &name) ) {
        return NULL;
    }
    self->py_name = name;

    Py_RETURN_NONE;
    return PyUnicode_FromFormat("%S %i", self->py_name);
}

static PyMethodDef py_script_methods[] = {
    {"get_name", (PyCFunction) py_script_get_name, METH_NOARGS, "Return the name, combining the first and last name" },
    {"set_name", (PyCFunction) py_script_set_name, METH_VARARGS | METH_KEYWORDS, "Set the name" },
    {"update_transform", (PyCFunction) py_script_set_transform, METH_VARARGS | METH_KEYWORDS, "Multiply args."},
    {"get_transform", (PyCFunction) py_script_get_transform, METH_NOARGS, "Multiply args."},
    //{"update_object_rotation", engine_updateobjectrotation, METH_VARARGS | METH_KEYWORDS, "Multiply args."},
    {NULL}  /* Sentinel */
};

// GETTERS AND SETTERS (properties)
PyObject* py_script_get_rotation(PythonScript *self, void *closure) {
    glm::quat q = self->parent->transform().get_rotation();
    //Py_INCREF(self->last);
    //#return self->last;
    return Py_BuildValue("ffff", q.x, q.y, q.z, q.w);
}

int py_script_set_rotation(PythonScript *self, PyObject *value, void *closure) {
    float x, y, z;
    if (!PyArg_ParseTuple(value, "fff", &x, &y, &z)) {
        return NULL;
    }

    glm::quat pitch = glm::angleAxis(x, glm::vec3(1, 0, 0));
    glm::quat yaw = glm::angleAxis(y, glm::vec3(0, 1, 0));
    glm::quat roll = glm::angleAxis(z, glm::vec3(0, 0, 1));

    glm::quat rotation = yaw * pitch * roll;

    self->parent->transform().set_rotation(rotation);

    return 0;
}

static PyGetSetDef py_script_getseters[] = {
    {"rotation", (getter)py_script_get_rotation, (setter)py_script_set_rotation, "Object rotation", NULL},
    //{"last", (getter)Noddy_getlast, (setter)Noddy_setlast, "last name", NULL},
    {NULL}  /* Sentinel */
};

/*
static PyTypeObject PyScriptType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "engine.Script",          tp_name 
    sizeof(Script),           tp_basicsize 
    0,                        tp_itemsize 
    (destructor) py_script_dealloc,   tp_dealloc 
    0,                        tp_print 
    0,                        tp_getattr 
    0,                        tp_setattr 
    0,                        tp_reserved 
    0,                        tp_repr 
    0,                        tp_as_number 
    0,                        tp_as_sequence 
    0,                        tp_as_mapping 
    0,                        tp_hash 
    0,                        tp_call 
    0,                        tp_str 
    0,                        tp_getattro 
    0,                        tp_setattro 
    0,                        tp_as_buffer 
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  tp_flags 
    "Engine PythonScriptObject",   tp_doc 
    0,                        tp_traverse 
    0,                        tp_clear 
    0,                        tp_richcompare 
    0,                        tp_weaklistoffset 
    0,                        tp_iter 
    0,                        tp_iternext 
    py_script_methods,        tp_methods 
    py_script_members,        tp_members 
    py_script_getseters,      tp_getset 
    0,                        tp_base 
    0,                        tp_dict 
    0,                        tp_descr_get 
    0,                        tp_descr_set 
    0,                        tp_dictoffset 
    (initproc)py_script_init,    tp_init 
    0,                        tp_alloc 
    py_script_new,               tp_new 
};*/

static PyTypeObject PyScriptType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "engine.Script", /* For printing, in format "<module>.<name>" */
    .tp_basicsize = sizeof(Script),
    .tp_itemsize = 0, /* For allocation */

    /* Methods to implement standard operations */
    .tp_dealloc = (destructor) py_script_dealloc,
    .tp_print = 0,
    .tp_getattr = 0,
    .tp_setattr = 0,
    .tp_as_async = 0,
    .tp_repr = 0,

    /* Method suites for standard classes */
    .tp_as_number = 0,
    .tp_as_sequence = 0,
    .tp_as_mapping = 0,

    /* More standard operations (here for binary compatibility) */
    .tp_hash = 0,
    .tp_call = 0,
    .tp_str = 0,
    .tp_getattro = 0,
    .tp_setattro = 0,

    /* Functions to access object as input/output buffer */
    .tp_as_buffer = 0,

    /* Flags to define presence of optional/expanded features */
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Engine PythonScriptObject", /* Documentation string */

    /* Assigned meaning in release 2.0 */
    /* call function for all accessible objects */
    .tp_traverse = 0,

    /* delete references to contained objects */
    .tp_clear = 0,

    /* Assigned meaning in release 2.1 */
    /* rich comparisons */
    .tp_richcompare = 0,

    /* weak reference enabler */
    .tp_weaklistoffset = 0,

    /* Iterators */
    .tp_iter = 0,
    .tp_iternext = 0,

    /* Attribute descriptor and subclassing stuff */
    .tp_methods = py_script_methods,
    .tp_members = py_script_members,
    .tp_getset = py_script_getseters,
    .tp_base = 0,
    .tp_dict = 0,
    .tp_descr_get = 0,
    .tp_descr_set = 0,
    .tp_dictoffset = 0,
    .tp_init = (initproc)py_script_init,
    .tp_alloc = 0,
    .tp_new = py_script_new,
    .tp_free = 0, /* Low-level free-memory routine */
    .tp_is_gc = 0, /* For PyObject_IS_GC */
    .tp_bases = 0,
    .tp_mro = 0, /* method resolution order */
    .tp_cache = 0,
    .tp_subclasses = 0,
    .tp_weaklist = 0,
    .tp_del = 0,

    /* Type attribute cache version tag. Added in version 2.6 */
    .tp_version_tag = 0,

    .tp_finalize = 0,
};

static PyModuleDef engine_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "engine",
    .m_doc = "Engine module for engine API.",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_py_script(void) {
    PyObject *m;
    if (PyType_Ready(&PyScriptType) < 0)
        return NULL;

    m = PyModule_Create(&engine_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyScriptType);
    if (PyModule_AddObject(m, "Script", (PyObject *) &PyScriptType) < 0) {
        Py_DECREF(&PyScriptType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

PyObject *script_class;

void script_init() {
    //Py_SetProgramName("EngineTest");
    //Py_SetPath(L"./"
    //PyImport_AppendInittab("engine_static", &PyInit_engine_static);
    PyImport_AppendInittab("engine", &PyInit_py_script);
    Py_Initialize();

    PyRun_SimpleString(
        "import sys\n"
        "sys.path.append('./')\n"
    );
    script_class = PyObject_GetAttrString(PyInit_py_script(), "Script");
}

PyObject *new_script() {
    ENGINE_ASSERT(Py_IsInitialized(), "Python is not initialised, cannot create PythonScript - call `script_init()`");
    PyObject *args = Py_BuildValue("(s)", "Python Script Instance");
    PyObject *script_instance = PyObject_CallObject(script_class, args);
    return script_instance;
}


} // namespace