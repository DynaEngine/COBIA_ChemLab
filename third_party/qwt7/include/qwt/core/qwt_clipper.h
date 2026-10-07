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

#ifndef QWT_CLIPPER_H
#define QWT_CLIPPER_H

#include "qwtcore_global.h"

class QwtInterval;
class QPointF;
class QRect;
class QRectF;
class QPolygon;
class QPolygonF;

#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief Namespace for clipping algorithms
 */
namespace QwtClipper
{
// Clip polygon against a rectangle
QWTCORE_EXPORT void clipPolygon(const QRect&, QPolygon&, bool closePolygon = false);

// Clip polygon against a float rectangle
QWTCORE_EXPORT void clipPolygon(const QRectF&, QPolygon&, bool closePolygon = false);

// Clip float polygon against a float rectangle
QWTCORE_EXPORT void clipPolygonF(const QRectF&, QPolygonF&, bool closePolygon = false);

// Return clipped polygon
QWTCORE_EXPORT QPolygon clippedPolygon(const QRect&, const QPolygon&, bool closePolygon = false);

// Return clipped polygon (float rect)
QWTCORE_EXPORT QPolygon clippedPolygon(const QRectF&, const QPolygon&, bool closePolygon = false);

// Return clipped float polygon
QWTCORE_EXPORT QPolygonF clippedPolygonF(const QRectF&, const QPolygonF&, bool closePolygon = false);

// Clip circle and return arcs as intervals
QWTCORE_EXPORT QVector< QwtInterval > clipCircle(const QRectF&, const QPointF&, double radius);
};

#endif
