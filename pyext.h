#include <Python.h>
#include "hashmap.h"

#pragma once

typedef struct OPy OPy;

struct OPy {
	OBJECTHEADER(ObjectType)
	PyObject *obj;
};

OPy *new_opy(PyObject *obj);
