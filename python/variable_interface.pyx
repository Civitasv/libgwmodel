cimport cbase
from cbase cimport GwmVariableInterface, GwmVariableListInterface
from libc.stdlib cimport malloc
from libc.string cimport strncpy, strcpy, memcpy

cdef class VariableInterface:
    cdef GwmVariableInterface _c_instance
    
    def __cinit__(self, int index, bint numeric, char[:] name):
        self._c_instance = GwmVariableInterface()
        self._c_instance.index = index
        self._c_instance.numeric = numeric
        strncpy(self._c_instance.name, &name[0], len(name))

    def __dealloc__(self):
        strcpy(self._c_instance.name, "")


cdef class VariableListInterface:
    cdef GwmVariableListInterface _c_instance
    
    def __cinit__(self, int size, VariableInterface[:] variables):
        cdef cbase.GwmVariableInterface* items = <cbase.GwmVariableInterface*>malloc(size * sizeof(cbase.GwmVariableInterface))
        cdef int i
        for i in range(size):
            items[i] = variables[i]._c_instance
        self._c_instance = GwmVariableListInterface(size, items)

    def __dealloc__(self):
        cbase.gwmodel_delete_variable_list(&self._c_instance)
    