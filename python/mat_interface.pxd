__all__ = ["MatInterface"]

from cbase cimport GwmMatInterface, gwmodel_delete_mat

cdef class MatInterface:
    cdef GwmMatInterface _c_instance
    
cdef MatInterface numpy2mat(double[:, ::1] array)
cdef mat2numpy(GwmMatInterface interface)