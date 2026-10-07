/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *
 * Modified by ChenZongYan in 2024 <czy.t@163.com>
 *   Summary of major modifications (see ChangeLog.md for full history):
 *   1. CMake build system & C++11 throughout.
 *   2. Core panner/ zoomer refactored:
 *        - QwtPanner → QwtCachePanner (pixmap-cache version)
 *        - New real-time QwtPlotPanner derived from QwtPicker.
 *   3. Zoomer supports multi-axis.
 *   4. Parasite-plot framework:
 *        - QwtFigure, QwtPlotParasiteLayout, QwtPlotTransparentCanvas,
 *        - QwtPlotScaleEventDispatcher, built-in pan/zoom on axis.
 *   5. New picker: QwtPlotSeriesDataPicker (works with date axis).
 *   6. Raster & color-map extensions:
 *        - QwtGridRasterData (2-D table + interpolation)
 *        - QwtLinearColorMap::stopColors(), stopPos() API rename.
 *   7. Bar-chart: expose pen/brush control.
 *   8. Amalgamated build: single QwtPlot.h / QwtPlot.cpp pair in src-amalgamate.
 *****************************************************************************/

#ifndef QWT_SERIES_DATA_H
#define QWT_SERIES_DATA_H

#include "qwtcore_global.h"
#include "qwt_samples.h"
#include "qwt_point_3d.h"

#include <qvector.h>
#include <qrect.h>

class QwtPointPolar;

/**
 * @brief Abstract interface for iterating over samples
 * @details Qwt offers several implementations of the QwtSeriesData API,
 *          but in situations, where data of an application specific format
 *          needs to be displayed, without having to copy it, it is recommended
 *          to implement an individual data access.
 *
 *          A subclass of QwtSeriesData<QPointF> must implement:
 *          - size(): Should return number of data points.
 *          - sample(): Should return values x and y values of the sample at specific position
 *            as QPointF object.
 *          - boundingRect(): Should return the bounding rectangle of the data series.
 *            It is used for autoscaling and might help certain algorithms for displaying
 *            the data. You can use qwtBoundingRect() for an implementation but often it is
 *            possible to implement a more efficient algorithm depending on the characteristics
 *            of the series. The member cachedBoundingRect is intended for caching the calculated rectangle.
 */
template< typename T >
class QwtSeriesData
{
public:
    /// Constructor
    QwtSeriesData();

    /// Destructor
    virtual ~QwtSeriesData();

#ifndef QWT_PYTHON_WRAPPER

    /// @return Number of samples
    virtual size_t size() const = 0;

    /// Return a sample
    virtual T sample(size_t i) const = 0;

    /// Calculate the bounding rect of all samples
    virtual QRectF boundingRect() const = 0;

#else
    // Needed for generating the python bindings, but not for using them !
    virtual size_t size() const
    {
        return 0;
    }
    virtual T sample(size_t i) const
    {
        return T();
    }
    virtual QRectF boundingRect() const
    {
        return cachedBoundingRect;
    }
#endif

    /// Set a the "rect of interest"
    virtual void setRectOfInterest(const QRectF& rect);

protected:
    /// Can be used to cache a calculated bounding rectangle
    mutable QRectF cachedBoundingRect;

private:
    QwtSeriesData< T >& operator=(const QwtSeriesData< T >&);
};

template< typename T >
QwtSeriesData< T >::QwtSeriesData() : cachedBoundingRect(0.0, 0.0, -1.0, -1.0)
{
}

template< typename T >
QwtSeriesData< T >::~QwtSeriesData()
{
}

template< typename T >
void QwtSeriesData< T >::setRectOfInterest(const QRectF&)
{
}

/**
 * @brief Template class for data, that is organized as QVector
 * @details QVector uses implicit data sharing and can be passed around as argument efficiently.
 */
template< typename T >
class QwtArraySeriesData : public QwtSeriesData< T >
{
public:
    /// Constructor
    QwtArraySeriesData();

    /// Constructor
    explicit QwtArraySeriesData(const QVector< T >& samples);
    explicit QwtArraySeriesData(QVector< T >&& samples);

    /// Assign an array of samples
    void setSamples(const QVector< T >& samples);
    void setSamples(QVector< T >&& samples);

    /// @return Array of samples
    const QVector< T > samples() const;

    /// @return Number of samples
    virtual size_t size() const override;

    /// @return Sample at a specific position
    virtual T sample(size_t index) const override;

protected:
    /// Vector of samples
    QVector< T > m_samples;
};

template< typename T >
QwtArraySeriesData< T >::QwtArraySeriesData()
{
}

template< typename T >
QwtArraySeriesData< T >::QwtArraySeriesData(const QVector< T >& samples) : m_samples(samples)
{
}

template< typename T >
QwtArraySeriesData< T >::QwtArraySeriesData(QVector< T >&& samples)
{
    m_samples = std::move(samples);
}

template< typename T >
void QwtArraySeriesData< T >::setSamples(const QVector< T >& samples)
{
    QwtSeriesData< T >::cachedBoundingRect = QRectF(0.0, 0.0, -1.0, -1.0);
    m_samples                              = samples;
}

template< typename T >
void QwtArraySeriesData< T >::setSamples(QVector< T >&& samples)
{
    QwtSeriesData< T >::cachedBoundingRect = QRectF(0.0, 0.0, -1.0, -1.0);
    m_samples                              = std::move(samples);
}

template< typename T >
const QVector< T > QwtArraySeriesData< T >::samples() const
{
    return m_samples;
}

template< typename T >
size_t QwtArraySeriesData< T >::size() const
{
    return m_samples.size();
}

template< typename T >
T QwtArraySeriesData< T >::sample(size_t i) const
{
    return m_samples[ static_cast< int >(i) ];
}

/**
 * @brief Interface for iterating over an array of points
 * @details QwtPointSeriesData provides access to QPointF samples stored in a QVector.
 */
class QWTCORE_EXPORT QwtPointSeriesData : public QwtArraySeriesData< QPointF >
{
public:
    //! Constructor
    QwtPointSeriesData(const QVector< QPointF >& = QVector< QPointF >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of 3D points
 * @details QwtPoint3DSeriesData provides access to QwtPoint3D samples stored in a QVector.
 */
class QWTCORE_EXPORT QwtPoint3DSeriesData : public QwtArraySeriesData< QwtPoint3D >
{
public:
    //! Constructor
    QwtPoint3DSeriesData(const QVector< QwtPoint3D >& = QVector< QwtPoint3D >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of intervals
 * @details QwtIntervalSeriesData provides access to QwtIntervalSample samples stored in a QVector.
 */
class QWTCORE_EXPORT QwtIntervalSeriesData : public QwtArraySeriesData< QwtIntervalSample >
{
public:
    //! Constructor
    QwtIntervalSeriesData(const QVector< QwtIntervalSample >& = QVector< QwtIntervalSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of set samples
 * @details QwtSetSeriesData provides access to QwtSetSample samples stored in a QVector.
 */
class QWTCORE_EXPORT QwtSetSeriesData : public QwtArraySeriesData< QwtSetSample >
{
public:
    //! Constructor
    QwtSetSeriesData(const QVector< QwtSetSample >& = QVector< QwtSetSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of vector field samples
 * @details QwtVectorFieldData provides access to QwtVectorFieldSample samples stored in a QVector.
 */
class QWTCORE_EXPORT QwtVectorFieldData : public QwtArraySeriesData< QwtVectorFieldSample >
{
public:
    //! Constructor
    QwtVectorFieldData(const QVector< QwtVectorFieldSample >& = QVector< QwtVectorFieldSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of OHLC samples
 * @details QwtTradingChartData provides access to QwtOHLCSample samples stored in a QVector.
 *          Used for candlestick or OHLC chart financial data.
 */
class QWTCORE_EXPORT QwtTradingChartData : public QwtArraySeriesData< QwtOHLCSample >
{
public:
    //! Constructor
    QwtTradingChartData(const QVector< QwtOHLCSample >& = QVector< QwtOHLCSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of boxplot samples
 * @details QwtBoxChartData provides access to QwtBoxSample samples stored in a QVector.
 *          Used for box-and-whisker plot statistical data.
 */
class QWTCORE_EXPORT QwtBoxChartData : public QwtArraySeriesData< QwtBoxSample >
{
public:
    //! Constructor
    QwtBoxChartData(const QVector< QwtBoxSample >& = QVector< QwtBoxSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;
};

/**
 * @brief Interface for iterating over an array of boxplot outlier samples
 * @details QwtBoxOutlierChartData provides access to QwtBoxOutlierSample samples stored in a QVector.
 *          Used for displaying outlier points in box-and-whisker plots.
 */
class QWTCORE_EXPORT QwtBoxOutlierChartData : public QwtArraySeriesData< QwtBoxOutlierSample >
{
public:
    //! Constructor
    QwtBoxOutlierChartData(const QVector< QwtBoxOutlierSample >& = QVector< QwtBoxOutlierSample >());

    //! Calculate the bounding rectangle
    virtual QRectF boundingRect() const override;

    //! Get total outlier count across all boxes
    int totalOutlierCount() const;
};

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QPointF >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtPoint3D >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtPointPolar >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtIntervalSample >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtSetSample >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtOHLCSample >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtVectorFieldSample >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtBoxSample >&, size_t from = 0, size_t to = 0);

QWTCORE_EXPORT QRectF qwtBoundingRect(const QwtSeriesData< QwtBoxOutlierSample >&, size_t from = 0, size_t to = 0);

/**
 * @brief Binary search for a sorted series of samples
 * @details qwtUpperSampleIndex returns the index of sample that is the upper bound
 *          of value. Is the the value smaller than the smallest value the return
 *          value will be 0. Is the value greater or equal than the largest
 *          value the return value will be series.size().
 *
 * @par Example
 * The following example shows finds a point of curve from an x coordinate
 * @code
 * #include <qwt_series_data.h>
 * #include <qwt_plot_curve.h>
 *
 *   struct compareX
 *   {
 *       inline bool operator()( const double x, const QPointF &pos ) const
 *       {
 *           return ( x < pos.x() );
 *       }
 *   };
 *
 *   QLineF curveLineAt( const QwtPlotCurve *curve, double x )
 *   {
 *       int index = qwtUpperSampleIndex<QPointF>(*curve->data(), x, compareX() );
 *
 *       if ( index == -1 &&
 *           x == curve->sample( curve->dataSize() - 1 ).x() )
 *       {
 *           // the last sample is excluded from qwtUpperSampleIndex
 *           index = curve->dataSize() - 1;
 *       }
 *
 *       QLineF line; // invalid
 *       if ( index > 0 )
 *       {
 *           line.setP1( curve->sample( index - 1 ) );
 *           line.setP2( curve->sample( index ) );
 *       }
 *
 *       return line;
 *   }
 *
 * @endcode
 * @endpar
 *
 * @param series Series of samples
 * @param value Value
 * @param lessThan Compare operation
 * @note The samples must be sorted according to the order specified by the lessThan object
 *
 * @note Starting from qwt7, this function is changed to size_t version. If not found,
 *       it will return series.size(), similar to an end iterator.
 */
template< typename T, typename LessThan >
inline size_t qwtUpperSampleIndex(const QwtSeriesData< T >& series, double value, LessThan lessThan)
{
    const size_t count = series.size();
    if (count == 0) {
        return count;  // Return 0 as "not found" marker (valid indices start from 0, count is beyond valid range)
    }

    const size_t indexMax = count - 1;

    // If value is greater than or equal to the last element, no element is greater than value, return count
    if (!lessThan(value, series.sample(indexMax))) {
        return count;
    }

    size_t indexMin = 0;
    size_t n        = indexMax;  // n represents the current search interval size

    while (n > 0) {
        const size_t half     = n >> 1;
        const size_t indexMid = indexMin + half;

        if (lessThan(value, series.sample(indexMid))) {
            // Target is in the left interval [indexMin, indexMid]
            n = half;
        } else {
            // Target is in the right interval [indexMid + 1, indexMin + n - 1]
            indexMin = indexMid + 1;
            n -= half + 1;
        }
    }

    return indexMin;
}

#endif
