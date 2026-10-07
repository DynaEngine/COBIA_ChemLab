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

#ifndef QWT_BEZIER_H
#define QWT_BEZIER_H

#include "qwtcore_global.h"

class QPointF;
class QPolygonF;

/**
 * @brief An implementation of the de Casteljau's Algorithm for interpolating Bézier curves
 * @details The flatness criterion for terminating the subdivision is based on
 *          "Piecewise Linear Approximation of Bézier Curves" by Roger Willcocks.
 *          See: https://jeremykun.com/2013/05/11/bezier-curves-and-picasso
 */
class QWTCORE_EXPORT QwtBezier
{
public:
    //! Constructor with tolerance parameter
    QwtBezier(double tolerance = 0.5);
    //! Destructor
    ~QwtBezier();

    //! Set the tolerance for curve subdivision
    void setTolerance(double tolerance);
    //! Get the tolerance value
    double tolerance() const;

    //! Interpolate a Bézier curve as a polygon
    QPolygonF toPolygon(const QPointF& p1, const QPointF& cp1, const QPointF& cp2, const QPointF& p2) const;

    //! Append Bézier curve points to an existing polygon
    void appendToPolygon(const QPointF& p1, const QPointF& cp1, const QPointF& cp2, const QPointF& p2, QPolygonF& polygon) const;

    //! Find a point on a Bézier curve at parameter t
    static QPointF pointAt(const QPointF& p1, const QPointF& cp1, const QPointF& cp2, const QPointF& p2, double t);

private:
    double m_tolerance;
    double m_flatness;
};

//! @return Tolerance value used for subdivision
inline double QwtBezier::tolerance() const
{
    return m_tolerance;
}

#endif
