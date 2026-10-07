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

#ifndef QWT_POINT_DATA_H
#define QWT_POINT_DATA_H

#include "qwtcore_global.h"
#include "qwt_series_data.h"

#include <cstring>

/**
 * @brief Interface for iterating over two QVector<T> objects.
 */
template< typename T >
class QwtPointArrayData : public QwtPointSeriesData
{
public:
    /// Constructor with QVector references
    QwtPointArrayData(const QVector< T >& x, const QVector< T >& y);
    /// Constructor with rvalue references
    QwtPointArrayData(QVector< T >&& x, QVector< T >&& y);
    /// Constructor with C-style arrays
    QwtPointArrayData(const T* x, const T* y, size_t size);

    /// Get the size of the data set
    virtual size_t size() const override;
    /// Get the sample at a specific index
    virtual QPointF sample(size_t index) const override;

    /// Get the x-data vector
    const QVector< T >& xData() const;
    /// Get the y-data vector
    const QVector< T >& yData() const;

private:
    QVector< T > m_x;
    QVector< T > m_y;
};

/**
 * @brief Data class containing two pointers to memory blocks of T.
 */
template< typename T >
class QwtCPointerData : public QwtPointSeriesData
{
public:
    /// Constructor with C-style arrays
    QwtCPointerData(const T* x, const T* y, size_t size);

    /// Get the size of the data set
    virtual size_t size() const override;
    /// Get the sample at a specific index
    virtual QPointF sample(size_t index) const override;

    /// Get the x-data pointer
    const T* xData() const;
    /// Get the y-data pointer
    const T* yData() const;

private:
    const T* m_x;
    const T* m_y;
    size_t m_size;
};

/**
 * @brief Interface for iterating over a QVector<T>.
 *
 * The memory contains the y coordinates, while the index is
 * interpreted as x coordinate.
 */
template< typename T >
class QwtValuePointData : public QwtPointSeriesData
{
public:
    /// Constructor with QVector reference
    QwtValuePointData(const QVector< T >& y);
    /// Constructor with C-style array
    QwtValuePointData(const T* y, size_t size);

    /// Get the size of the data set
    virtual size_t size() const override;
    /// Get the sample at a specific index
    virtual QPointF sample(size_t index) const override;

    /// Get the y-data vector
    const QVector< T >& yData() const;

private:
    QVector< T > m_y;
};

/**
 * @brief Data class containing a pointer to memory of y coordinates
 *
 * The memory contains the y coordinates, while the index is
 * interpreted as x coordinate.
 */
template< typename T >
class QwtCPointerValueData : public QwtPointSeriesData
{
public:
    /// Constructor with C-style array
    QwtCPointerValueData(const T* y, size_t size);

    /// Get the size of the data set
    virtual size_t size() const override;
    /// Get the sample at a specific index
    virtual QPointF sample(size_t index) const override;

    /// Get the y-data pointer
    const T* yData() const;

private:
    const T* m_y;
    size_t m_size;
};

/**
 * @brief Synthetic point data
 *
 * QwtSyntheticPointData provides a fixed number of points for an interval.
 * The points are calculated in equidistant steps in x-direction.
 *
 * If the interval is invalid, the points are calculated for
 * the "rectangle of interest", what normally is the displayed area on the
 * plot canvas. In this mode you get different levels of detail, when
 * zooming in/out.
 *
 * @par Example
 *
 * The following example shows how to implement a sinus curve.
 *
 * @code
 * #include <cmath>
 * #include <qwt_series_data.h>
 * #include <qwt_plot_curve.h>
 * #include <qwt_plot.h>
 * #include <qapplication.h>
 *
 * class SinusData: public QwtSyntheticPointData
 * {
 * public:
 *  SinusData():
 *      QwtSyntheticPointData( 100 )
 *  {
 *  }
 *
 *  virtual double y( double x ) const
 *  {
 *      return qSin( x );
 *  }
 * };
 *
 * int main(int argc, char **argv)
 * {
 *  QApplication a( argc, argv );
 *
 *  QwtPlot plot;
 *  plot.setAxisScale( QwtAxis::XBottom, 0.0, 10.0 );
 *  plot.setAxisScale( QwtAxis::YLeft, -1.0, 1.0 );
 *
 *  QwtPlotCurve *curve = new QwtPlotCurve( "y = sin(x)" );
 *  curve->setData( new SinusData() );
 *  curve->attach( &plot );
 *
 *  plot.show();
 *  return a.exec();
 * }
 * @endcode
 */
class QWTCORE_EXPORT QwtSyntheticPointData : public QwtPointSeriesData
{
public:
    // Constructor
    QwtSyntheticPointData(size_t size, const QwtInterval& = QwtInterval());

    // Set the number of points
    void setSize(size_t size);
    // Get the number of points
    virtual size_t size() const override;

    // Set the interval
    void setInterval(const QwtInterval&);
    // Get the interval
    QwtInterval interval() const;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;
    // Get the sample at a specific index
    virtual QPointF sample(size_t index) const override;

    // Calculate a y value for a x value
    virtual double y(double x) const = 0;
    // Calculate the x value for a given index
    virtual double x(size_t index) const;

    // Set the rectangle of interest
    virtual void setRectOfInterest(const QRectF&) override;
    // Get the rectangle of interest
    QRectF rectOfInterest() const;

private:
    size_t m_size;
    QwtInterval m_interval;
    QRectF m_rectOfInterest;
    QwtInterval m_intervalOfInterest;
};

/// Constructor with QVector references
template< typename T >
QwtPointArrayData< T >::QwtPointArrayData(const QVector< T >& x, const QVector< T >& y) : m_x(x), m_y(y)
{
}

/// Constructor with rvalue references
template< typename T >
QwtPointArrayData< T >::QwtPointArrayData(QVector< T >&& x, QVector< T >&& y)
{
    m_x = std::move(x);
    m_y = std::move(y);
}

/// Constructor with C-style arrays
template< typename T >
QwtPointArrayData< T >::QwtPointArrayData(const T* x, const T* y, size_t size)
{
    m_x.resize(size);
    std::memcpy(m_x.data(), x, size * sizeof(T));

    m_y.resize(size);
    std::memcpy(m_y.data(), y, size * sizeof(T));
}

/// Get the size of the data set
template< typename T >
size_t QwtPointArrayData< T >::size() const
{
    return qMin(m_x.size(), m_y.size());
}

/// Get the sample at a specific index
template< typename T >
QPointF QwtPointArrayData< T >::sample(size_t index) const
{
    return QPointF(m_x[ int(index) ], m_y[ int(index) ]);
}

/// Get the x-data vector
template< typename T >
const QVector< T >& QwtPointArrayData< T >::xData() const
{
    return m_x;
}

/// Get the y-data vector
template< typename T >
const QVector< T >& QwtPointArrayData< T >::yData() const
{
    return m_y;
}

/// Constructor with QVector reference
template< typename T >
QwtValuePointData< T >::QwtValuePointData(const QVector< T >& y) : m_y(y)
{
}

/// Constructor with C-style array
template< typename T >
QwtValuePointData< T >::QwtValuePointData(const T* y, size_t size)
{
    m_y.resize(size);
    std::memcpy(m_y.data(), y, size * sizeof(T));
}

/// Get the size of the data set
template< typename T >
size_t QwtValuePointData< T >::size() const
{
    return m_y.size();
}

/// Get the sample at a specific index
template< typename T >
QPointF QwtValuePointData< T >::sample(size_t index) const
{
    return QPointF(index, m_y[ int(index) ]);
}

/// Get the y-data vector
template< typename T >
const QVector< T >& QwtValuePointData< T >::yData() const
{
    return m_y;
}

/// Constructor with C-style arrays
template< typename T >
QwtCPointerData< T >::QwtCPointerData(const T* x, const T* y, size_t size) : m_x(x), m_y(y), m_size(size)
{
}

/// Get the size of the data set
template< typename T >
size_t QwtCPointerData< T >::size() const
{
    return m_size;
}

/// Get the sample at a specific index
template< typename T >
QPointF QwtCPointerData< T >::sample(size_t index) const
{
    return QPointF(m_x[ int(index) ], m_y[ int(index) ]);
}

/// Get the x-data pointer
template< typename T >
const T* QwtCPointerData< T >::xData() const
{
    return m_x;
}

/// Get the y-data pointer
template< typename T >
const T* QwtCPointerData< T >::yData() const
{
    return m_y;
}

/// Constructor with C-style array
template< typename T >
QwtCPointerValueData< T >::QwtCPointerValueData(const T* y, size_t size) : m_y(y), m_size(size)
{
}

/// Get the size of the data set
template< typename T >
size_t QwtCPointerValueData< T >::size() const
{
    return m_size;
}

/// Get the sample at a specific index
template< typename T >
QPointF QwtCPointerValueData< T >::sample(size_t index) const
{
    return QPointF(index, m_y[ int(index) ]);
}

/// Get the y-data pointer
template< typename T >
const T* QwtCPointerValueData< T >::yData() const
{
    return m_y;
}

#endif
