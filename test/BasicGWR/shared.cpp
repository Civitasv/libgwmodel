#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <vector>
#include <string>
#include <armadillo>
#include <gwmodel.h>
#include "londonhp100.h"

using namespace std;
using namespace arma;

TEST_CASE("BasicGWR: basic flow")
{
    mat londonhp100_coord, londonhp100_data;
    vector<string> londonhp100_fields;
    if (!read_londonhp100(londonhp100_coord, londonhp100_data, londonhp100_fields))
    {
        FAIL("Cannot load londonhp100 data.");
    }

    GwmMatInterface londonhp100_coord_interface = { londonhp100_coord.n_rows, londonhp100_coord.n_cols, londonhp100_coord.memptr() };
    GwmMatInterface londonhp100_data_interface = { londonhp100_data.n_rows, londonhp100_data.n_cols, londonhp100_data.memptr() };
    GwmStringListInterface londonhp100_field_interface;
    londonhp100_field_interface.size = londonhp100_fields.size();
    londonhp100_field_interface.items = new GwmStringInterface[londonhp100_field_interface.size];
    for (int i = 0; i < londonhp100_fields.size(); i++)
    {
        GwmStringInterface* s = londonhp100_field_interface.items + i;
        s->str = new char[londonhp100_fields[i].size() + 1];
        strcpy((char*)s->str, londonhp100_fields[i].data());
    }

    
    CGwmSimpleLayer* londonhp = gwmodel_create_simple_layer(londonhp100_coord_interface, londonhp100_data_interface, londonhp100_field_interface);

    CGwmDistance* distance = gwmodel_create_crs_distance(false);
    CGwmWeight* bandwidth = gwmodel_create_bandwidth_weight(36, true, KernelFunctionType::Gaussian);
    CGwmSpatialWeight* spatial = gwmodel_create_spatial_weight(distance, bandwidth);

    GwmVariableInterface purchase = { 0, true, "PURCHASE" };
    GwmVariableInterface floorsz = { 1, true, "FLOORSZ" };
    GwmVariableInterface unemploy = { 2, true, "UNEMPLOY" };
    GwmVariableInterface prof = { 3, true, "PROF" };
    GwmVariableListInterface indepVars;
    indepVars.size = 3;
    indepVars.items = new GwmVariableInterface[indepVars.size];
    indepVars.items[0] = floorsz;
    indepVars.items[1] = unemploy;
    indepVars.items[2] = prof;

    CGwmGWRBasic* algorithm = gwmodel_create_gwr_algorithm();
    gwmodel_set_gwr_source_layer(algorithm, londonhp);
    gwmodel_set_gwr_dependent_variable(algorithm, purchase);
    gwmodel_set_gwr_independent_variable(algorithm, indepVars);
    gwmodel_set_gwr_spatial_weight(algorithm, spatial);
    gwmodel_set_gwr_options(algorithm, true);
    REQUIRE_NOTHROW(gwmodel_run_gwr(algorithm));

    GwmRegressionDiagnostic diagnostic = gwmodel_get_gwr_diagnostic(algorithm);
    REQUIRE(abs(diagnostic.AIC - 2436.60445730413) < 1e-8);
    REQUIRE(abs(diagnostic.AICc - 2448.27206524754) < 1e-8);
    REQUIRE(abs(diagnostic.RSquare - 0.708010632044736) < 1e-8);
    REQUIRE(abs(diagnostic.RSquareAdjust - 0.674975341723766) < 1e-8);
}

TEST_CASE("BasicGWR: adaptive bandwidth autoselection of with CV")
{
    mat londonhp100_coord, londonhp100_data;
    vector<string> londonhp100_fields;
    if (!read_londonhp100(londonhp100_coord, londonhp100_data, londonhp100_fields))
    {
        FAIL("Cannot load londonhp100 data.");
    }

    GwmMatInterface londonhp100_coord_interface = { londonhp100_coord.n_rows, londonhp100_coord.n_cols, londonhp100_coord.memptr() };
    GwmMatInterface londonhp100_data_interface = { londonhp100_data.n_rows, londonhp100_data.n_cols, londonhp100_data.memptr() };
    GwmStringListInterface londonhp100_field_interface;
    londonhp100_field_interface.size = londonhp100_fields.size();
    londonhp100_field_interface.items = new GwmStringInterface[londonhp100_field_interface.size];
    for (int i = 0; i < londonhp100_fields.size(); i++)
    {
        GwmStringInterface* s = londonhp100_field_interface.items + i;
        s->str = new char[londonhp100_fields[i].size() + 1];
        strcpy((char*)s->str, londonhp100_fields[i].data());
    }

    
    CGwmSimpleLayer* londonhp = gwmodel_create_simple_layer(londonhp100_coord_interface, londonhp100_data_interface, londonhp100_field_interface);

    CGwmDistance* distance = gwmodel_create_crs_distance(false);
    CGwmWeight* bandwidth = gwmodel_create_bandwidth_weight(36, true, KernelFunctionType::Gaussian);
    CGwmSpatialWeight* spatial = gwmodel_create_spatial_weight(distance, bandwidth);

    GwmVariableInterface purchase = { 0, true, "PURCHASE" };
    GwmVariableInterface floorsz = { 1, true, "FLOORSZ" };
    GwmVariableInterface unemploy = { 2, true, "UNEMPLOY" };
    GwmVariableInterface prof = { 3, true, "PROF" };
    GwmVariableListInterface indepVars;
    indepVars.size = 3;
    indepVars.items = new GwmVariableInterface[indepVars.size];
    indepVars.items[0] = floorsz;
    indepVars.items[1] = unemploy;
    indepVars.items[2] = prof;

    CGwmGWRBasic* algorithm = gwmodel_create_gwr_algorithm();
    gwmodel_set_gwr_source_layer(algorithm, londonhp);
    gwmodel_set_gwr_dependent_variable(algorithm, purchase);
    gwmodel_set_gwr_independent_variable(algorithm, indepVars);
    gwmodel_set_gwr_spatial_weight(algorithm, spatial);
    gwmodel_set_gwr_options(algorithm, true);
    gwmodel_set_gwr_bandwidth_autoselection(algorithm, BandwidthSelectionCriterionType::CV);
    REQUIRE_NOTHROW(gwmodel_run_gwr(algorithm));


    CGwmSpatialWeight* spatial_new = gwmodel_get_gwr_spatial_weight(algorithm);

    GwmBandwidthKernelInterface bw_new_interf;
    if (gwmodel_get_spatial_bandwidth_weight(spatial_new, &bw_new_interf))
    {
        REQUIRE(bw_new_interf.size == 67);
    }
    else
    {
        FAIL("Cannot regard a CGwmWeight instance as a CGwmBandwidthWeight instance!");
    }
}

vector<int> variables2indices(const GwmVariableListInterface& variables)
{
    vector<int> index;
    for (size_t i = 0; i < variables.size; i++)
    {
        index.push_back(variables.items[i].index);
    }
    return index;
}

TEST_CASE("BasicGWR: indepdent variable autoselection with AIC")
{
    mat londonhp100_coord, londonhp100_data;
    vector<string> londonhp100_fields;
    if (!read_londonhp100(londonhp100_coord, londonhp100_data, londonhp100_fields))
    {
        FAIL("Cannot load londonhp100 data.");
    }

    GwmMatInterface londonhp100_coord_interface = { londonhp100_coord.n_rows, londonhp100_coord.n_cols, londonhp100_coord.memptr() };
    GwmMatInterface londonhp100_data_interface = { londonhp100_data.n_rows, londonhp100_data.n_cols, londonhp100_data.memptr() };
    GwmStringListInterface londonhp100_field_interface;
    londonhp100_field_interface.size = londonhp100_fields.size();
    londonhp100_field_interface.items = new GwmStringInterface[londonhp100_field_interface.size];
    for (int i = 0; i < londonhp100_fields.size(); i++)
    {
        GwmStringInterface* s = londonhp100_field_interface.items + i;
        s->str = new char[londonhp100_fields[i].size() + 1];
        strcpy((char*)s->str, londonhp100_fields[i].data());
    }

    
    CGwmSimpleLayer* londonhp = gwmodel_create_simple_layer(londonhp100_coord_interface, londonhp100_data_interface, londonhp100_field_interface);

    CGwmDistance* distance = gwmodel_create_crs_distance(false);
    CGwmWeight* bandwidth = gwmodel_create_bandwidth_weight(36, true, KernelFunctionType::Gaussian);
    CGwmSpatialWeight* spatial = gwmodel_create_spatial_weight(distance, bandwidth);

    GwmVariableInterface purchase = { 0, true, "PURCHASE" };
    GwmVariableInterface floorsz = { 1, true, "FLOORSZ" };
    GwmVariableInterface unemploy = { 2, true, "UNEMPLOY" };
    GwmVariableInterface prof = { 3, true, "PROF" };
    GwmVariableListInterface indepVars;
    indepVars.size = 3;
    indepVars.items = new GwmVariableInterface[indepVars.size];
    indepVars.items[0] = floorsz;
    indepVars.items[1] = unemploy;
    indepVars.items[2] = prof;

    CGwmGWRBasic* algorithm = gwmodel_create_gwr_algorithm();
    gwmodel_set_gwr_source_layer(algorithm, londonhp);
    gwmodel_set_gwr_dependent_variable(algorithm, purchase);
    gwmodel_set_gwr_independent_variable(algorithm, indepVars);
    gwmodel_set_gwr_spatial_weight(algorithm, spatial);
    gwmodel_set_gwr_options(algorithm, true);
    gwmodel_set_gwr_indep_vars_autoselection(algorithm, 3.0);
    REQUIRE_NOTHROW(gwmodel_run_gwr(algorithm));

    GwmVariablesCriterionListInterface criterionList = gwmodel_get_gwr_indep_var_criterions(algorithm);
    
    REQUIRE_THAT(variables2indices(criterionList.items[0].variables), Catch::Equals(vector<int>({ 2 })));
    REQUIRE_THAT(criterionList.items[0].criterion, Catch::WithinAbs(2551.61359020599, 1e-8));
    REQUIRE_THAT(variables2indices(criterionList.items[1].variables), Catch::Equals(vector<int>({ 3 })));
    REQUIRE_THAT(criterionList.items[1].criterion, Catch::WithinAbs(2551.30032201349, 1e-8));
    REQUIRE_THAT(variables2indices(criterionList.items[2].variables), Catch::Equals(vector<int>({ 1 })));
    REQUIRE_THAT(criterionList.items[2].criterion, Catch::WithinAbs(2468.93236280013, 1e-8));
    REQUIRE_THAT(variables2indices(criterionList.items[3].variables), Catch::Equals(vector<int>({ 1, 3 })));
    REQUIRE_THAT(criterionList.items[3].criterion, Catch::WithinAbs(2452.86447942033, 1e-8));
    REQUIRE_THAT(variables2indices(criterionList.items[4].variables), Catch::Equals(vector<int>({ 1, 2 })));
    REQUIRE_THAT(criterionList.items[4].criterion, Catch::WithinAbs(2450.59642666509, 1e-8));
    REQUIRE_THAT(variables2indices(criterionList.items[5].variables), Catch::Equals(vector<int>({ 1, 2, 3 })));
    REQUIRE_THAT(criterionList.items[5].criterion, Catch::WithinAbs(2452.80388934625, 1e-8));
}