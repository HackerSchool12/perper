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


OPy *new_opy(PyObject *obj) {
	OPy *opy = malloc(sizeof(OPy));
	opy->proto.hash = PyObject_Hash(obj);
	opy->proto.class = OPY;
	opy->proto.equal = opy_equal;
	opy->obj = obj;
	opy->proto.refcount = 1;
	opy->proto.free = opy_free;
	return opy;
}
