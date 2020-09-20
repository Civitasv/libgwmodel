#include "gwmodel.h"

#include <vector>
#include <armadillo>
#include "GwmVariable.h"
#include "spatialweight/CGwmCRSDistance.h"
#include "spatialweight/CGwmBandwidthWeight.h"
#include "spatialweight/CGwmSpatialWeight.h"
#include "CGwmSimpleLayer.h"
#include "CGwmSpatialAlgorithm.h"
#include "CGwmSpatialMonoscaleAlgorithm.h"
#include "CGwmGWRBase.h"
#include "CGwmGWRBasic.h"

using namespace std;
using namespace arma;

CGwmSpatialAlgorithm* gwmodel_create_algorithm()
{
    return nullptr;
}

CGwmDistance* gwmodel_create_crs_distance(bool isGeogrphical)
{
    return new CGwmCRSDistance(isGeogrphical);
}

CGwmWeight* gwmodel_create_bandwidth_weight(double size, bool isAdaptive, KernelFunctionType type)
{
    return new CGwmBandwidthWeight(size, isAdaptive, (CGwmBandwidthWeight::KernelFunctionType)type);
}

CGwmSpatialWeight* gwmodel_create_spatial_weight(CGwmDistance* distance, CGwmWeight* weight)
{
    return new CGwmSpatialWeight(weight, distance);
}

CGwmSimpleLayer* gwmodel_create_simple_layer(GwmMatInterface pointsInterface, GwmMatInterface dataInterface, GwmStringListInterface fieldsInterface)
{
    mat points(pointsInterface.data, pointsInterface.rows, pointsInterface.cols);
    mat data(dataInterface.data, dataInterface.rows, dataInterface.cols);
    vector<string> fields;
    for (size_t i = 0; i < fieldsInterface.size; i++)
    {
        GwmStringInterface* s = fieldsInterface.items + i;
        printf("%s", s->str);
        fields.push_back(string(s->str));
    }
    return new CGwmSimpleLayer(points, data, fields);
}

CGwmGWRBasic* gwmodel_create_gwr_algorithm()
{
    CGwmGWRBasic* algorithm = new CGwmGWRBasic();
    return algorithm;
}

void gwmodel_delete_crs_distance(CGwmDistance* instance)
{
    delete instance;
}

void gwmodel_delete_bandwidth_weight(CGwmWeight* instance)
{
    delete instance;
}

void gwmodel_delete_spatial_weight(CGwmSpatialWeight* instance)
{
    delete instance;
}

void gwmodel_delete_simple_layer(CGwmSimpleLayer* instance)
{
    delete instance;
}

void gwmodel_delete_algorithm(CGwmSpatialAlgorithm* instance)
{
    delete instance;
}

void gwmodel_delete_gwr_algorithm(CGwmGWRBasic* instance)
{
    delete instance;
}

void gwmodel_set_gwr_source_layer(CGwmGWRBasic* algorithm, CGwmSimpleLayer* layer)
{
    algorithm->setSourceLayer(layer);
}

void gwmodel_set_gwr_predict_layer(CGwmGWRBasic* algorithm, CGwmSimpleLayer* layer)
{
    algorithm->setPredictLayer(layer);
}

void gwmodel_set_gwr_dependent_variable(CGwmGWRBasic* regression, GwmVariableInterface depVar)
{
    regression->setDependentVariable(GwmVariable(depVar.index, depVar.isNumeric, depVar.name));
}

void gwmodel_set_gwr_independent_variable(CGwmGWRBasic* regression, GwmVariableListInterface indepVarList)
{
    vector<GwmVariable> indepVars;
    for (size_t i = 0; i < indepVarList.size; i++)
    {
        GwmVariableInterface* vi = indepVarList.items + i;
        _ASSERT(vi);
        GwmVariable v(vi->index, vi->isNumeric, vi->name);
        indepVars.push_back(v);
    }
    regression->setIndependentVariables(indepVars);
}

void gwmodel_set_gwr_spatial_weight(CGwmGWRBasic* algorithm, CGwmSpatialWeight* spatial)
{
    algorithm->setSpatialWeight(*spatial);
}

void gwmodel_set_gwr_bandwidth_autoselection(CGwmGWRBasic* algorithm, BandwidthSelectionCriterionType criterion)
{
    algorithm->setIsAutoselectBandwidth(true);
    algorithm->setBandwidthSelectionCriterion((CGwmGWRBasic::BandwidthSelectionCriterionType)criterion);
}

void gwmodel_set_gwr_indep_vars_autoselection(CGwmGWRBasic* algorithm, double threshold)
{
    algorithm->setIsAutoselectIndepVars(true);
    algorithm->setIndepVarSelectionThreshold(threshold);
}

void gwmodel_set_gwr_options(CGwmGWRBasic* algorithm, bool hasHatMatrix)
{
    algorithm->setHasHatMatrix(hasHatMatrix);
}

void gwmodel_run_gwr(CGwmGWRBasic* algorithm)
{
    algorithm->run();
}

void gwmodel_get_simple_layer_points(CGwmSimpleLayer* layer, GwmMatInterface* pointsInterface)
{
    mat points = layer->points();
    pointsInterface->rows = points.n_rows;
    pointsInterface->cols = points.n_cols;
    pointsInterface->data = new double[points.n_elem];
    memcpy(pointsInterface->data, points.memptr(), points.n_elem * sizeof(double));
}

void gwmodel_get_simple_layer_data(CGwmSimpleLayer* layer, GwmMatInterface* dataInterface)
{
    mat data = layer->data();
    dataInterface->rows = data.n_rows;
    dataInterface->cols = data.n_cols;
    dataInterface->data = new double[data.n_elem];
    memcpy(dataInterface->data, data.memptr(), data.n_elem * sizeof(double));
}

void gwmodel_get_simple_layer_fields(CGwmSimpleLayer* layer, GwmStringListInterface* fieldsInterface)
{
    vector<string> fields = layer->fields();
    fieldsInterface->size = fields.size();
    fieldsInterface->items = new GwmStringInterface[fieldsInterface->size];
    for (size_t i = 0; i < fieldsInterface->size; i++)
    {
        string f = fields[i];
        GwmStringInterface* s = fieldsInterface->items + i;
        s->str = new char[f.size() + 1];
        strcpy((char*)s->str, f.data());
    }
}

void gwmodel_get_gwr_spatial_weight(CGwmGWRBasic* gwr, CGwmDistance** distance, CGwmWeight** weight)
{
    *distance = gwr->spatialWeight().distance()->clone();
    *weight = gwr->spatialWeight().weight()->clone();
}

void gwmodel_get_gwr_result_layer(CGwmGWRBasic* gwr, CGwmSimpleLayer** resultLayer)
{
    *resultLayer = gwr->resultLayer();
}

void gwmodel_get_gwr_coefficients(CGwmGWRBasic* gwr, GwmMatInterface* coefficientsInterface)
{
    mat betas = gwr->betas();
    coefficientsInterface->rows = betas.n_rows;
    coefficientsInterface->cols = betas.n_cols;
    coefficientsInterface->data = new double[betas.n_elem];
    memcpy(coefficientsInterface->data, betas.memptr(), betas.n_elem * sizeof(double));
}

GwmRegressionDiagnostic gwmodel_get_gwr_diagnostic(CGwmGWRBasic* gwr)
{
    return gwr->diagnostic();
}

bool gwmodel_as_bandwidth_weight(CGwmWeight* weight, GwmBandwidthKernelInterface* bandwidth)
{
    CGwmBandwidthWeight* bw = dynamic_cast<CGwmBandwidthWeight*>(weight);
    if (bw)
    {
        *bandwidth = { bw->bandwidth(), bw->adaptive(), (KernelFunctionType)bw->kernel() };
        return true;
    }
    else return false;
}