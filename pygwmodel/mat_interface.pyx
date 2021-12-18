from .mat_interface cimport GwmMatInterface, gwmodel_delete_mat
from libc.stdlib cimport malloc, free
from cython cimport view
import numpy as np

cdef class MatInterface:
    def __cinit__(self, rows, cols, double[::1, :] array):
        self._c_instance = GwmMatInterface(rows, cols, &array[0, 0])

    def __dealloc__(self):
        gwmodel_delete_mat(&self._c_instance)

    def show(self):
        cdef unsigned long long i, j
        for i in range(self._c_instance.rows):
            for j in range(self._c_instance.cols):
                print(self._c_instance.data[i * self._c_instance.cols + j], end="\t")
            print()


cpdef MatInterface numpy2mat(double[::1, :] array):
    cdef unsigned long long rows = array.shape[0]
    cdef unsigned long long cols = array.shape[1]
    cdef double* ptr = <double*>malloc(cols * rows * sizeof(double))
    cdef unsigned long long i, j
    for i in range(cols):
        for j in range(rows):
            ptr[i * rows + j] = array[j, i]
    cdef view.array data = view.array(shape=(cols, rows), itemsize=sizeof(double), format="d", mode="fortran", allocate_buffer=False)
    data.data = <char*>ptr
    return MatInterface(rows, cols, data)

cdef MatInterface mat2interface(GwmMatInterface interface):
    cdef unsigned long long rows = interface.rows
    cdef unsigned long long cols = interface.cols
    cdef view.array data = view.array(shape=(rows, cols), itemsize=sizeof(double), format="d", mode="fortran", allocate_buffer=False)
    data.data = <char*>interface.data
    return MatInterface(rows, cols, data)

cdef mat2numpy(GwmMatInterface interface):
    cdef const double* src = interface.data
    cdef unsigned long long rows = interface.rows
    cdef unsigned long long cols = interface.cols
    result = np.zeros((rows, cols), dtype=np.float64, order="C")
    cdef double[:, ::1] dst = result
    cdef unsigned long long i, j
    for i in range(rows):
        for j in range(cols):
            dst[i, j] = src[j * interface.rows + i]
    return result