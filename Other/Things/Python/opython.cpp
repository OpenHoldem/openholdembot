#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <conio.h>
#include <windows.h>
#include <string>
#include "OpenHoldemFunctions.h"

PyObject *pName = nullptr, *pModule = nullptr;
PyObject *pFunc = nullptr;

static PyObject* getSymbol(PyObject *self, PyObject *args)
{
	char *symbol;
	if (!PyArg_ParseTuple(args, "s", &symbol))
	{
		_cprintf("could not parse getSymbol arg");
		return NULL;
	}

	double res = GetSymbol(symbol);
	_cprintf("getSymbol %s: %f\n", symbol, res);
	return Py_BuildValue("d", res);
}

struct module_state {
	PyObject *error;
};

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

static PyObject* error_out(PyObject *m)
{
	struct module_state *st = GETSTATE(m);
	PyErr_SetString(st->error, "something bad happened");
	return NULL;
}

static PyMethodDef openholdem_methods[] = {
	{ "getSymbol", (PyCFunction)getSymbol, METH_VARARGS, "Get OpenHoldem symbol" },
	{ NULL, NULL, 0, NULL }
};

static int openholdem_traverse(PyObject *m, visitproc visit, void *arg)
{
	Py_VISIT(GETSTATE(m)->error);
	return 0;
}

static int openholdem_clear(PyObject *m)
{
	Py_CLEAR(GETSTATE(m)->error);
	return 0;
}

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"OpenHoldem",
	NULL,
	sizeof(struct module_state),
	openholdem_methods,
	NULL,
	openholdem_traverse,
	openholdem_clear,
	NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC PyInit_openholdem(void)
{
	PyObject *module = PyModule_Create(&moduledef);

	if (module == NULL)
		INITERROR;

	struct module_state *st = GETSTATE(module);
	st->error = PyErr_NewException("OpenHoldem.Error", NULL, NULL);
	if (st->error == NULL)
	{
		Py_DECREF(module);
		INITERROR;
	}

	return module;
}

int pyInit()
{
	PyImport_AppendInittab("OpenHoldem", &PyInit_openholdem);
	Py_Initialize();

	pName = PyUnicode_FromString("pyVoodoo");
	pModule = PyImport_Import(pName);
	Py_XDECREF(pName);

	if (pModule)
	{
		pFunc = PyObject_GetAttrString(pModule, "getDecision");
		if (pFunc && PyCallable_Check(pFunc))
		{
			_cprintf("found getDecision()\n");
		}
		else
		{
			_cprintf("ERROR: function getDecision()\n");
			return 1;
		}
	}
	else
	{
		_cprintf("ERROR: Module not imported\n");
		return 1;
	}
	
	return 0;
}

int pyDestroy()
{
	if (pFunc)
		Py_XDECREF(pFunc);

	if(pModule)
		Py_XDECREF(pModule);

	Py_Finalize();

	return 0;
}

double pyDecision()
{
	PyObject *pValue;
	double ret = 0;

	pValue = PyObject_CallObject(pFunc, NULL);
	ret = PyFloat_AsDouble(pValue);

	_cprintf("getDecision() = %f\n", ret);
	Py_XDECREF(pValue);

	return ret;
}