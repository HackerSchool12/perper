from cpython cimport PyObject, Py_INCREF, Py_DECREF

cdef extern from "object.h":
    ctypedef struct Object:
        pass

    void retain(Object *obj)
    void release(Object *obj)

cdef extern from "pyext.h":
    ctypedef struct OPy:
        PyObject *obj
    
    OPy *new_opy(object obj)

cdef extern from "hashmap.h":
    ctypedef Object *(*finder)(BasicNode *self, int level, Object *key)
    ctypedef BasicNode *(*inserter)(BasicNode *self, int level, Object *key, Object *value)
    ctypedef BasicNode *(*remover)(BasicNode *self, int level, Object *key)

    ctypedef struct NodeType:
        finder find
        inserter insert
        remover remove

    ctypedef struct BasicNode:
        NodeType *class_ "class"


    BasicNode *new_empty_node()

cdef class PersistentDict(object):
    cdef BasicNode *cdict

    def __init__(self):
        self.cdict = new_empty_node()

    def __getitem__(self, key):
        ckey = <Object*>new_opy(key)
        cval = <OPy*>self.cdict.class_.find(self.cdict, 0, ckey)
        release(ckey)
        if cval != NULL:
            return <object>cval.obj

    def setitem(self, key, value):
        ckey = <Object*>new_opy(key)
        cval = <Object*>new_opy(value)
        newdict = <BasicNode*>self.cdict.class_.insert(self.cdict, 0, ckey, cval)
        release(ckey)
        release(cval)
        pydict = PersistentDict()
        pydict.cdict = newdict
        return pydict

    def delitem(self, key):
        ckey = <Object*>new_opy(key)
        newdict = <BasicNode*>self.cdict.class_.remove(self.cdict, 0, ckey)
        release(ckey)
        pydict = PersistentDict()
        pydict.cdict = newdict
        return pydict

    def __dealloc__(self):
        release(<Object*>self.cdict)
