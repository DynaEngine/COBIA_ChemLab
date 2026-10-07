/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_TRANSFORM_H
#define QWT_PLOT_TRANSFORM_H

#include "qwt_global.h"
#include "qwt_axis_id.h"

#include <qpoint.h>
#include <qrect.h>

class QPainterPath;
class QwtPlot;

/**
 * @brief Utility class for coordinate transformations on a QwtPlot
 * @details QwtPlotTransform provides static methods to convert between different
 *          coordinate systems, compute pixel offsets, and determine the visible
 *          data range. All methods are read-only and do not modify the plot.
 *
 *          Coordinate systems in Qwt:
 *          - Data coordinates: the actual x/y values of the plotted data
 *          - Screen coordinates: pixel positions on the canvas widget
 *          - Cross-axis transformation: converting data from one axis pair to
 *            another while preserving the same screen position
 *
 * @code
 * QwtPlot* plot = ...;
 * QRectF visible = Qwt::QwtPlotTransform::visibleRange(plot);
 * qDebug() << "Visible range:" << visible;
 *
 * // Convert a mouse click to data coordinates
 * QPointF dataPoint = Qwt::QwtPlotTransform::toPlotPoint(plot, mousePos);
 * @endcode
 *
 * @sa QwtPlot::invTransform(), QwtPlot::transform(), QwtScaleMap
 */
class QWT_EXPORT QwtPlotTransform
{
public:
    // ---- Cross-axis transformation ----

    // Transform a point from one axis pair to another (same screen position)
    static QPointF
    transformPoint(const QwtPlot* plot, const QPointF& point, QwtAxisId fromX, QwtAxisId fromY, QwtAxisId toX, QwtAxisId toY);

    // Transform a path from one axis pair to another
    static QPainterPath
    transformPath(const QwtPlot* plot, const QPainterPath& path, QwtAxisId fromX, QwtAxisId fromY, QwtAxisId toX, QwtAxisId toY);

    // ---- Screen / data coordinate conversion ----

    // Convert screen position to data coordinates
    static QPointF toPlotPoint(const QwtPlot* plot,
                               const QPointF& screenPos,
                               QwtAxisId xAxis = QwtAxis::XBottom,
                               QwtAxisId yAxis = QwtAxis::YLeft);

    // Convert data coordinates to screen position
    static QPointF toScreenPoint(const QwtPlot* plot,
                                 const QPointF& plotPoint,
                                 QwtAxisId xAxis = QwtAxis::XBottom,
                                 QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Pixel offset ----

    // Compute the data-coordinate offset corresponding to 1 pixel
    static QPointF onePixelOffset(const QwtPlot* plot, QwtAxisId xAxis = QwtAxis::XBottom, QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Visible range ----

    // Get the currently visible data range
    static QRectF visibleRange(const QwtPlot* plot, QwtAxisId xAxis = QwtAxis::XBottom, QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Total data range ----

    // Get the union of all data item bounding rectangles
    static QRectF totalDataRange(const QwtPlot* plot, bool onlyVisible = true);
};

#endif  // QWT_PLOT_TRANSFORM_H
