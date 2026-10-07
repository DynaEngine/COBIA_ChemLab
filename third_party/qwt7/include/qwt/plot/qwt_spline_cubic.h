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

#ifndef QWT_SPLINE_CUBIC_H
#define QWT_SPLINE_CUBIC_H

#include "qwt_global.h"
#include "qwt_spline.h"

/**
 * @brief A cubic spline
 *
 * A cubic spline is a spline with C2 continuity at all control points.
 * It is a non local spline, where a modification of one control point
 * affects the whole spline.
 *
 * The implementation is based on the equation system of the
 * second derivatives M(x) of the control points P(x).
 *
 * QwtSplineCubic offers several algorithms for finding M(x), that
 * are selected by setBoundaryType().
 *
 * The default setting is a "natural spline" having M(x0) = M(xn) = 0.
 *
 * @sa QwtSpline::BoundaryType
 */
class QWT_EXPORT QwtSplineCubic : public QwtSplineC2
{
public:
    //! Constructor
    QwtSplineCubic();
    //! Destructor
    ~QwtSplineCubic() override;

    //! Get locality (always 0 - non-local)
    virtual uint locality() const override;

    //! Get painter path from polygon
    virtual QPainterPath painterPath(const QPolygonF&) const override;
    //! Get Bezier control lines
    virtual QVector< QLineF > bezierControlLines(const QPolygonF& points) const override;

    //! Get polynomials from polygon
    virtual QVector< QwtSplinePolynomial > polynomials(const QPolygonF&) const override;
    //! Get slopes at control points
    virtual QVector< double > slopes(const QPolygonF&) const override;
    //! Get curvatures at control points
    virtual QVector< double > curvatures(const QPolygonF&) const override;

private:
    QWT_DECLARE_PRIVATE(QwtSplineCubic)
};

#endif
