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
#ifndef QWT_POINT_POLAR_H
#define QWT_POINT_POLAR_H

#include "qwtcore_global.h"
#include "qwt_math.h"

#include <qpoint.h>
#include <qmetatype.h>
#include <qmath.h>

/**
 * @brief A point in polar coordinates
 * @details In polar coordinates a point is determined by an angle and a distance.
 *          See http://en.wikipedia.org/wiki/Polar_coordinate_system
 */

class QWTCORE_EXPORT QwtPointPolar
{
public:
    QwtPointPolar();
    QwtPointPolar(double azimuth, double radius);
    QwtPointPolar(const QPointF&);

    void setPoint(const QPointF&);
    QPointF toPoint() const;

    bool isValid() const noexcept;
    bool isNull() const noexcept;

    double radius() const noexcept;
    double azimuth() const noexcept;

    double& rRadius() noexcept;
    double& rAzimuth() noexcept;

    void setRadius(double) noexcept;
    void setAzimuth(double) noexcept;

    bool operator==(const QwtPointPolar&) const;
    bool operator!=(const QwtPointPolar&) const;

    QwtPointPolar normalized() const;

private:
    double m_azimuth { 0.0 };
    double m_radius { 0.0 };
};

Q_DECLARE_TYPEINFO(QwtPointPolar, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(QwtPointPolar);

#ifndef QT_NO_DEBUG_STREAM
QWTCORE_EXPORT QDebug operator<<(QDebug, const QwtPointPolar&);
#endif

/**
 * @brief Default constructor - constructs a null point with radius and azimuth set to 0.0
 * @sa QPointF::isNull()
 */
inline QwtPointPolar::QwtPointPolar() : m_azimuth(0.0), m_radius(0.0)
{
}

/**
 * @brief Constructs a point with coordinates specified by radius and azimuth
 * @param azimuth Azimuth (angle)
 * @param radius Radius (distance)
 */
inline QwtPointPolar::QwtPointPolar(double azimuth, double radius) : m_azimuth(azimuth), m_radius(radius)
{
}

//! Return true if radius() >= 0.0
inline bool QwtPointPolar::isValid() const noexcept
{
    return m_radius >= 0.0;
}

//! Return true if radius() == 0.0
inline bool QwtPointPolar::isNull() const noexcept
{
    return m_radius == 0.0;
}

//! Return the radius
inline double QwtPointPolar::radius() const noexcept
{
    return m_radius;
}

//! Return the azimuth
inline double QwtPointPolar::azimuth() const noexcept
{
    return m_azimuth;
}

//! Return a reference to the radius
inline double& QwtPointPolar::rRadius() noexcept
{
    return m_radius;
}

//! Return a reference to the azimuth
inline double& QwtPointPolar::rAzimuth() noexcept
{
    return m_azimuth;
}

//! Set the radius to radius
inline void QwtPointPolar::setRadius(double radius) noexcept
{
    m_radius = radius;
}

//! Set the azimuth to azimuth
inline void QwtPointPolar::setAzimuth(double azimuth) noexcept
{
    m_azimuth = azimuth;
}

inline QPoint qwtPolar2Pos(const QPoint& pole, double radius, double angle)
{
    const double x = pole.x() + radius * std::cos(angle);
    const double y = pole.y() - radius * std::sin(angle);

    return QPoint(qRound(x), qRound(y));
}

inline QPoint qwtDegree2Pos(const QPoint& pole, double radius, double angle)
{
    return qwtPolar2Pos(pole, radius, angle / 180.0 * M_PI);
}

inline QPointF qwtPolar2Pos(const QPointF& pole, double radius, double angle)
{
    const double x = pole.x() + radius * std::cos(angle);
    const double y = pole.y() - radius * std::sin(angle);

    return QPointF(x, y);
}

inline QPointF qwtDegree2Pos(const QPointF& pole, double radius, double angle)
{
    return qwtPolar2Pos(pole, radius, angle / 180.0 * M_PI);
}

inline QPointF qwtFastPolar2Pos(const QPointF& pole, double radius, double angle)
{
    const double x = pole.x() + radius * qFastCos(angle);
    const double y = pole.y() - radius * qFastSin(angle);

    return QPointF(x, y);
}

inline QPointF qwtFastDegree2Pos(const QPointF& pole, double radius, double angle)
{
    return qwtFastPolar2Pos(pole, radius, angle / 180.0 * M_PI);
}

inline QwtPointPolar qwtFastPos2Polar(const QPointF& pos)
{
    return QwtPointPolar(qwtFastAtan2(pos.y(), pos.x()), qSqrt(qwtSqr(pos.x()) + qwtSqr(pos.y())));
}

#endif
