#include "pyext.h"

bool opy_equal(Object *self, Object *other) {
	return (bool)(
		self->class == other->class
		&& PyObject_RichCompareBool(
			((OPy*)self)->obj,
			((OPy*)other)->obj,
			Py_EQ));
}

void opy_free(Object *obj) {
	OPy *self = (OPy*)obj;
	Py_DECREF(self->obj);
	free(self);
}

ObjectType opy_type = {opy_equal, opy_free};

OPy *new_opy(PyObject *obj) {
	Py_INCREF(obj);
	OPy *opy = malloc(sizeof(OPy));
	opy->hash = PyObject_Hash(obj);
	opy->class = &opy_type;
	opy->obj = obj;
	opy->refcount = 1;
	return opy;
}
