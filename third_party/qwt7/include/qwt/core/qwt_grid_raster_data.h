#ifndef QWT_GRID_RASTER_DATA_H
#define QWT_GRID_RASTER_DATA_H
#include "qwtcore_global.h"
#include "qwt_raster_data.h"
#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief A class that encapsulates grid data and provides interpolation methods.
 * @details This class inherits from QwtRasterData and is used to represent 2D grid data.
 *          It supports various interpolation methods such as nearest neighbor and bilinear interpolation.
 */
class QWTCORE_EXPORT QwtGridRasterData : public QwtRasterData
{
public:
    /**
     * @brief Resampling algorithm
     * @details The default setting is NearestNeighbour.
     *
     */
    enum ResampleMode
    {
        /**
         * Return the value from the matrix that is nearest to the requested position.
         *
         */
        NearestNeighbour,

        /**
         * Interpolate the value from the distances and values of the 4 surrounding values in the matrix.
         *
         */
        BilinearInterpolation,

        /**
         * Interpolate the value from the 16 surrounding values in the matrix using hermite bicubic interpolation.
         *
         */
        BicubicInterpolation
    };

public:
    // Constructor
    QwtGridRasterData();
    // Destructor
    ~QwtGridRasterData() override;

    // Set the resampling algorithm
    void setResampleMode(ResampleMode mode);
    // Return the resampling algorithm
    ResampleMode resampleMode() const;

    // Return bounding interval for an axis
    virtual QwtInterval interval(Qt::Axis axis) const override final;

    // Set new x-axis, y-axis, and data matrix
    void setValue(const QVector< double >& x, const QVector< double >& y, const QVector< QVector< double > >& v);
    // Return the value at a raster position
    virtual double value(double x, double y) const override;

    // Calculate the pixel hint
    virtual QRectF pixelHint(const QRectF&) const override;

    // Return the size of x-axis
    int xSize() const;
    // Return the size of y-axis
    int ySize() const;
    // Return the size of the value matrix
    std::pair< int, int > valueSize() const;

    // Return the value at specified position in the value matrix
    double atValue(int xIndex, int yIndex) const;

    // Return the x-axis value at specified index
    double atX(int xIndex) const;
    // Return the y-axis value at specified index
    double atY(int yIndex) const;

private:
    QWT_DECLARE_PRIVATE(QwtGridRasterData)
};

#endif  // QWTGRIDRASTERDATA_H
