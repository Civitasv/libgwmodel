cdef extern from "gwmodel.h":
    cdef struct GwmMatInterface:
        unsigned long long rows
        unsigned long long cols
        const double* data
    cdef void gwmodel_delete_mat(GwmMatInterface* interface)

    ctypedef char GwmNameInterface[256]

    cdef struct GwmNameListInterface:
        size_t size
        GwmNameInterface* items
    cdef void gwmodel_delete_string_list(GwmNameListInterface* interface)

    cdef struct GwmVariableInterface:
        int index
        bint isNumeric
        GwmNameInterface name
    
    cdef struct GwmVariableListInterface:
        size_t size
        GwmVariableInterface* items
    cdef void gwmodel_delete_variable_list(GwmVariableListInterface* interface)
    
    ctypedef struct CGwmSimpleLayer:
        pass
    cdef CGwmSimpleLayer* gwmodel_create_simple_layer(GwmMatInterface points, GwmMatInterface data, GwmNameListInterface fields)
    cdef void gwmodel_delete_simple_layer(CGwmSimpleLayer* instance)
    cdef GwmMatInterface gwmodel_get_simple_layer_points(CGwmSimpleLayer* layer)
    cdef GwmMatInterface gwmodel_get_simple_layer_data(CGwmSimpleLayer* layer)
    cdef GwmNameListInterface gwmodel_get_simple_layer_fields(CGwmSimpleLayer* layer)

    cdef enum KernelFunctionType:
        Gaussian
        Exponential
        Bisquare
        Tricube
        Boxcar

    ctypedef struct CGwmDistance:
        pass
    cdef CGwmDistance* gwmodel_create_crs_distance(bint isGeogrphical)
    cdef void gwmodel_delete_crs_distance(CGwmDistance* instance)

    ctypedef struct CGwmWeight:
        pass
    cdef CGwmWeight* gwmodel_create_bandwidth_weight(double size, bint isAdaptive, KernelFunctionType kernel)
    cdef void gwmodel_delete_bandwidth_weight(CGwmWeight* instance)

    ctypedef struct CGwmSpatialWeight:
        pass
    cdef CGwmSpatialWeight* gwmodel_create_spatial_weight(CGwmDistance* distance, CGwmWeight* weight)
    cdef void gwmodel_delete_spatial_weight(CGwmSpatialWeight* instance)

    ctypedef struct CGwmGWSS:
        pass
    cdef CGwmGWSS* gwmodel_create_gwss_algorithm()
    cdef void gwmodel_delete_gwss_algorithm(CGwmGWSS* instance)
    cdef CGwmSimpleLayer* gwmodel_get_gwss_result_layer(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_mean(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_sdev(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_var(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_skew(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_cv(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_cov(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_corr(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_spearman_rho(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_median(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_iqr(CGwmGWSS* gwss)
    cdef GwmMatInterface gwmodel_get_gwss_local_qi(CGwmGWSS* gwss)
    cdef void gwmodel_set_gwss_source_layer(CGwmGWSS* algorithm, CGwmSimpleLayer* layer)
    cdef void gwmodel_set_gwss_spatial_weight(CGwmGWSS* algorithm, CGwmSpatialWeight* spatial)
    cdef void gwmodel_set_gwss_variables(CGwmGWSS* algorithm, GwmVariableListInterface varList)
    cdef void gwmodel_set_gwss_options(CGwmGWSS* algorithm, bint quantile, bint corrWithFirstOnly)
    cdef void gwmodel_set_gwss_openmp(CGwmGWSS* algorithm, int threads)
    cdef void gwmodel_run_gwss(CGwmGWSS* algorithm)