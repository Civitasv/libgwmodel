#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include <vector>
#include <string>
#include <armadillo>


#include "gwmodelpp/spatialweight/CRSSTDistance.h"
#include "gwmodelpp/spatialweight/BandwidthWeight.h"
#include "gwmodelpp/spatialweight/SpatialWeight.h"
#include "gwmodelpp/GTWR.h"

#include "include/londonhp100.h"

using namespace std;
using namespace arma;
using namespace gwm;

TEST_CASE("GTWR: basic flow")
{
    mat londonhp100_coord, londonhp100_data;
    vector<string> londonhp100_fields;
    if (!read_londonhp100temporal(londonhp100_coord, londonhp100_data, londonhp100_fields))
    {
        FAIL("Cannot load londonhp100temporal data.");
    }

    CRSDistance sdist(true);
    OneDimDistance tdist;
    CRSSTDistance distance(&sdist, &tdist, 0.5);
    BandwidthWeight bandwidth(36,true, BandwidthWeight::Gaussian);
    SpatialWeight spatial(&bandwidth, &distance);

    vec y = londonhp100_data.col(0);
    mat x = join_rows(ones(londonhp100_coord.n_rows), londonhp100_data.cols(1, 3));

    GTWR algorithm;
    algorithm.setCoords(londonhp100_coord);
    algorithm.setDependentVariable(y);
    algorithm.setIndependentVariables(x);
    algorithm.setSpatialWeight(spatial);
    algorithm.setHasHatMatrix(true);
    REQUIRE_NOTHROW(algorithm.fit());

    RegressionDiagnostic diagnostic = algorithm.diagnostic();
    REQUIRE_THAT(diagnostic.AIC, Catch::Matchers::WithinAbs(2443.9698348782, 1e-8));
    REQUIRE_THAT(diagnostic.AICc, Catch::Matchers::WithinAbs(2456.3750569354, 1e-8));
    REQUIRE_THAT(diagnostic.RSquare, Catch::Matchers::WithinAbs(0.6872921780938, 1e-8));
    REQUIRE_THAT(diagnostic.RSquareAdjust, Catch::Matchers::WithinAbs(0.65184964517969, 1e-8));

    REQUIRE(algorithm.hasIntercept() == true);
}