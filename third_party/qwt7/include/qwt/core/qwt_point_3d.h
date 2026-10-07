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

/*! @file */
#ifndef QWT_POINT_3D_H
#define QWT_POINT_3D_H

#include "qwtcore_global.h"
#include <qpoint.h>
#include <qmetatype.h>

/**
 * @brief QwtPoint3D class defines a 3D point in double coordinates
 */

class QWTCORE_EXPORT QwtPoint3D
{
public:
    QwtPoint3D();
    QwtPoint3D(double x, double y, double z);
    QwtPoint3D(const QPointF&);

    bool isNull() const noexcept;

    double x() const noexcept;
    double y() const noexcept;
    double z() const noexcept;

    double& rx() noexcept;
    double& ry() noexcept;
    double& rz() noexcept;

    void setX(double x) noexcept;
    void setY(double y) noexcept;
    void setZ(double y) noexcept;

    QPointF toPoint() const noexcept;

    bool operator==(const QwtPoint3D&) const noexcept;
    bool operator!=(const QwtPoint3D&) const noexcept;

private:
    double m_x { 0.0 };
    double m_y { 0.0 };
    double m_z { 0.0 };
};

Q_DECLARE_TYPEINFO(QwtPoint3D, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(QwtPoint3D);

#ifndef QT_NO_DEBUG_STREAM
QWTCORE_EXPORT QDebug operator<<(QDebug, const QwtPoint3D&);
#endif

/**
 * @brief Default constructor - constructs a null point
 * @sa isNull()
 */
inline QwtPoint3D::QwtPoint3D() : m_x(0.0), m_y(0.0), m_z(0.0)
{
}

/**
 * @brief Constructs a point with coordinates specified by x, y and z
 * @param x X coordinate
 * @param y Y coordinate
 * @param z Z coordinate (default 0.0)
 */
inline QwtPoint3D::QwtPoint3D(double x, double y, double z = 0.0) : m_x(x), m_y(y), m_z(z)
{
}

/**
 * @brief Constructs a point with x and y coordinates from a 2D point, and z coordinate of 0
 * @param other 2D point
 */
inline QwtPoint3D::QwtPoint3D(const QPointF& other) : m_x(other.x()), m_y(other.y()), m_z(0.0)
{
}

/**
 * @return True if the point is null (x, y, z all equal to zero); otherwise false
 */
inline bool QwtPoint3D::isNull() const noexcept
{
    return m_x == 0.0 && m_y == 0.0 && m_z == 0.0;
}

//! Return the x-coordinate of the point
inline double QwtPoint3D::x() const noexcept
{
    return m_x;
}

//! Return the y-coordinate of the point
inline double QwtPoint3D::y() const noexcept
{
    return m_y;
}

//! Return the z-coordinate of the point
inline double QwtPoint3D::z() const noexcept
{
    return m_z;
}

//! Return a reference to the x-coordinate of the point
inline double& QwtPoint3D::rx() noexcept
{
    return m_x;
}

//! Return a reference to the y-coordinate of the point
inline double& QwtPoint3D::ry() noexcept
{
    return m_y;
}

//! Return a reference to the z-coordinate of the point
inline double& QwtPoint3D::rz() noexcept
{
    return m_z;
}

//! Set the x-coordinate of the point to x
inline void QwtPoint3D::setX(double x) noexcept
{
    m_x = x;
}

//! Set the y-coordinate of the point to y
inline void QwtPoint3D::setY(double y) noexcept
{
    m_y = y;
}

//! Set the z-coordinate of the point to z
inline void QwtPoint3D::setZ(double z) noexcept
{
    m_z = z;
}

/**
 * @return 2D point with the z coordinate dropped
 */
inline QPointF QwtPoint3D::toPoint() const noexcept
{
    return QPointF(m_x, m_y);
}

//! Return true if this point and other are equal
inline bool QwtPoint3D::operator==(const QwtPoint3D& other) const noexcept
{
    return (m_x == other.m_x) && (m_y == other.m_y) && (m_z == other.m_z);
}

//! Return true if this point and other are different
inline bool QwtPoint3D::operator!=(const QwtPoint3D& other) const noexcept
{
    return !operator==(other);
}

#endif
