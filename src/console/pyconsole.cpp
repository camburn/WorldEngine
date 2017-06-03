#include "pyconsole.hpp"

PyObject *console_instance;
const char *sp = ">>> ";
// TODO: There must be a nicer way to do this
char command_result[1000];

const char *GetSP() {
    return sp;
}

void ConsoleInit() {
    PyObject *pName, *pyModule;
    pName = PyUnicode_FromString("console.console");
    pyModule = PyImport_Import(pName);
    PyObject *console = PyObject_GetAttrString(pyModule, "Console");
    console_instance = PyObject_CallObject(console, NULL);
    Py_DecRef(console);
    if (console_instance == NULL) {
        exit(1);
    }
}

const char *SendCommand(const char *command) {
    // TODO: Handle when someone exits the interpreter with `exit()`
    PyObject *result = PyObject_CallMethod(console_instance, "run_command", "(s)", command);
    if (result != NULL && PyTuple_Check(result)) {
        sp = PyUnicode_AsUTF8(PyTuple_GetItem(result, 0));
        char *eval = PyUnicode_AsUTF8(PyTuple_GetItem(result, 1));
        strcpy(command_result, eval);
        Py_DecRef(result);
        return command_result;
    }
    Py_DecRef(result);
    return "Failed to run command";
}
