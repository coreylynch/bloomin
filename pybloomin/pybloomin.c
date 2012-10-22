#include <Python.h>
#include "bloom.h"
#include "structmember.h"

int Py_ModuleVersion = 1;

typedef struct {
	PyObject_HEAD
	BLOOM *filter;
} Bloomin;


static int Bloomin_init(Bloomin *self, PyObject *args, PyObject *kwds)
{
	double error_rate;
	unsigned int size;
	unsigned int num_funcs = 2; /* using two hash functions at the moment, so set to two */
	static char *kwlist[] = {"size", "error_rate", NULL};

	if (! PyArg_ParseTupleAndKeywords(args, kwds, "|id", kwlist,
										&size, &error_rate)){
		return -1;
	}

	self->filter = bloom_create(size, error_rate);

	return 0;
}

static void Bloomin_dealloc(Bloomin *self)
{
	bloom_destroy(self->filter);
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *Bloomin_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Bloomin *self;

	if ((self = (Bloomin *)type->tp_alloc(type, 0)) == NULL){
		return NULL;
	}

	self->filter = NULL;

	return (PyObject *) self;
}


static int contains(Bloomin *self, PyObject *key)
{
	if (!PyString_Check(key)) {
		return 0;
	}
	return bloom_check(self->filter,
						PyString_AsString(key),
						(int)PyString_Size(key));
}


static PyObject *check(Bloomin *self, PyObject *args)
{
	const char *s;
	int len;

	if (!PyArg_ParseTuple(args, "s#", &s, &len))
	{
		return NULL;
	}
	return Py_BuildValue("i", bloom_check(self->filter, s, len));
}

static PyObject *add(Bloomin *self, PyObject *args)
{
	const char *s;
	int len;
	
	if (! PyArg_ParseTuple(args, "s#", &s, &len))
	{
		return NULL;
	}

	return Py_BuildValue("i", bloom_insert(self->filter, s, len));
}

static PyObject *delete(Bloomin *self, PyObject *args)
{
	const char *s;
	int len;
	if (! PyArg_ParseTuple(args, "s#", &s, &len)){
		return NULL;
	}

	return Py_BuildValue("i", bloom_delete(self->filter, s, len));
}

static PyMethodDef Bloomin_methods[] = {
	{"add",         (PyCFunction)add,         METH_VARARGS | METH_KEYWORDS, "Add an element to the bloom filter."},
    {"delete",      (PyCFunction)delete,      METH_VARARGS | METH_KEYWORDS, "Remove an element from the bloom filter."},
    {"check",       (PyCFunction)check,       METH_VARARGS | METH_KEYWORDS, "Check if an element is in the bloom filter."},
    {NULL},       /* Sentinel */
};

static PyMemberDef Bloomin_members[] = {
    {NULL}  /* Sentinel */
};

static PySequenceMethods Bloomin_sequence = {
  NULL,                 /*sq_length*/
  NULL,                 /*sq_concat*/
  NULL,                 /*sq_repeat*/
  NULL,                 /*sq_item*/
  NULL,                 /*sq_slice*/
  NULL,                 /*sq_ass_item*/
  NULL,                 /*sq_ass_slice*/
  (objobjproc)contains, /*sq_contains*/
};

static PyTypeObject BloominType = {
	PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "bloomin.Bloomin",          /*tp_name*/
    sizeof(Bloomin),               /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor)Bloomin_dealloc,   /*tp_dealloc*/
    0,                              /*tp_print*/
    0,                              /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
    0,                              /*tp_repr*/
    0,                              /*tp_as_number*/
    &Bloomin_sequence,             /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash*/
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    0,                              /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    "Bloomin objects",             /*tp_doc*/
    0,                              /*tp_traverse*/
    0,                              /*tp_clear*/
    0,                              /*tp_richcompare*/
    0,                              /*tp_weaklistoffset*/
    0,                              /*tp_iter*/
    0,                              /*tp_iternext*/
    Bloomin_methods,               /*tp_methods*/
    Bloomin_members,               /*tp_members*/
    0,                              /*tp_getset*/
    0,                              /*tp_base*/
    0,                              /*tp_dict*/
    0,                              /*tp_descr_get*/
    0,                              /*tp_descr_set*/
    0,                              /*tp_dictoffset*/
    (initproc)Bloomin_init,        /*tp_init*/
    0,                              /*tp_alloc*/
    Bloomin_new,                   /*tp_new*/
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC initpybloomin(void)
{
	PyObject *m;
	if (PyType_Ready(&BloominType) <0){
		return;
	}

	m = Py_InitModule3("pybloomin", Bloomin_methods, "Bloomin module");

	if (m == NULL){
		return;
	}

	Py_INCREF(&BloominType);
    PyModule_AddObject(m, "Bloomin", (PyObject *)&BloominType);
}



























