#ifndef CGWMOneDimDISTANCE_H
#define CGWMOneDimDISTANCE_H

#include "CGwmDistance.h"

/**
 * @brief Class for calculating spatial distance according to coordinate reference system.
 */
class CGwmOneDimDistance : public CGwmDistance
{
public:

    /**
     * @brief Struct of parameters used in spatial distance calculating according to coordinate reference system. 
     * Usually a pointer to object of this class is passed to CGwmOneDimDistance::distance().
     */
    struct Parameter : public CGwmDistance::Parameter
    {
        arma::vec focusPoints;    //!< Matrix of focus points' coordinates. The shape of it must be nx2 and the first column is longitudes or x-coordinate, the second column is latitudes or y-coordinate.
        arma::vec dataPoints;     //!< Matrix of data points' coordinates. The shape of it must be nx2 and the first column is longitudes or x-coordinate, the second column is latitudes or y-coordinate.

        /**
         * @brief Construct a new OneDimDistanceParameter object.
         * 
         * @param fp Reference to focus points.
         * @param dp Reference to data points.
         */
        Parameter(const arma::vec& fp, const arma::vec& dp) : CGwmDistance::Parameter()
            , focusPoints(fp)
            , dataPoints(dp)
        {
            total = fp.n_rows;
        }
    };

public:

    /**
     * @brief Calculate spatial distance for points with geographical coordinate reference system.
     * 
     * @param out_loc Matrix of focus point' coordinate. The shape of it must be 1x2 and the first column is longitudes, the second column is latitudes.
     * @param in_locs Matrix of data points' coordinates. The shape of it must be nx2 and the first column is longitudes, the second column is latitudes.
     * @return arma::vec Distance vector for out_loc.
     */
    static arma::vec AbstractDistance(const double& out_loc, const arma::vec& in_locs)
    {
        // arma::vec d = abs(in_locs - out_loc);
        // d.print("d");
        return abs(in_locs - out_loc);
    }

public:

    CGwmOneDimDistance();

    /**
     * @brief Construct a new CGwmOneDimDistance object
     * 
     * @param isGeographic Whether the coordinate reference system is geographical.
     */
    explicit CGwmOneDimDistance(bool isGeographic);

    /**
     * @brief Construct a new CGwmOneDimDistance object.
     * 
     * @param distance Refernce to object for copying.
     */
    CGwmOneDimDistance(const CGwmOneDimDistance& distance);

    virtual CGwmDistance * clone() const override
    {
        return new CGwmOneDimDistance(*this);
    }

    DistanceType type() const override { return DistanceType::OneDimDistance; }

public:

    /**
     * @brief Create Parameter for Caclulating CRS Distance.
     * 
     * @param plist A list of parameters containing 2 items:
     *  - `arma::vec` focus coordinates (one column)
     *  - `arma::vec` data coordinates (one column)
     *  .
     * 
     * @return DistanceParameter* The pointer to parameters.
     */
    virtual void makeParameter(std::initializer_list<DistParamVariant> plist) override;

    virtual arma::vec distance(arma::uword focus) const override;
    virtual double maxDistance() const override;
    virtual double minDistance() const override;

protected:
    std::unique_ptr<Parameter> mParameter = nullptr;
};

#endif // CGWMOneDimDISTANCE_H
