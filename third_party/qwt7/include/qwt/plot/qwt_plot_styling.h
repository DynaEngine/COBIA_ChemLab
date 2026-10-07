/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_STYLING_H
#define QWT_PLOT_STYLING_H

#include "qwt_global.h"
#include "qwt_symbol.h"

#include <qbrush.h>
#include <qcolor.h>
#include <qpen.h>
#include <qsize.h>

class QwtPlot;
class QwtPlotItem;
class QwtPlotCurve;

/**
 * @brief Utility class for reading and writing visual style properties of QwtPlotItem
 * @details QwtPlotStyling provides type-safe static methods to get or set the color,
 *          pen, brush, and symbol of any QwtPlotItem, dispatching through the item's
 *          rtti to call the correct type-specific API.
 *
 *          Also provides convenience methods for common styling operations such as
 *          setting a curve's symbol or recommending pen widths based on data density.
 *
 * @code
 * QwtPlotItem* item = ...;
 *
 * // Get color regardless of item type
 * QColor c = Qwt::QwtPlotStyling::color(item);
 *
 * // Set color uniformly
 * Qwt::QwtPlotStyling::setColor(item, Qt::red);
 *
 * // Add a symbol to a curve
 * Qwt::QwtPlotStyling::setSymbol(curve, QwtSymbol::Ellipse, Qt::blue, QSize(6, 6));
 * @endcode
 *
 * @sa QwtPlotItemInfo, QwtPlotDataAccess
 */
class QWT_EXPORT QwtPlotStyling
{
public:
    // ---- Color ----

    // Get the representative color of a plot item
    static QColor color(const QwtPlotItem* item, const QColor& defaultColor = QColor());

    // Set the color of a plot item (dispatches to pen or brush)
    static bool setColor(QwtPlotItem* item, const QColor& color);

    // ---- Pen ----

    // Get the pen of a plot item
    static QPen pen(const QwtPlotItem* item);

    // Set the pen of a plot item
    static bool setPen(QwtPlotItem* item, const QPen& pen);

    // ---- Brush ----

    // Get the brush of a plot item
    static QBrush brush(const QwtPlotItem* item);

    // Set the brush of a plot item
    static bool setBrush(QwtPlotItem* item, const QBrush& brush);

    // ---- Curve symbol ----

    // Set a symbol on a curve with style and size
    static void setSymbol(QwtPlotCurve* curve, QwtSymbol::Style style, const QSize& size = QSize(8, 8));

    // Set a symbol on a curve with style, color and size
    static void setSymbol(QwtPlotCurve* curve, QwtSymbol::Style style, const QColor& color, const QSize& size = QSize(8, 8));

    // ---- Curve line style ----

    // Set curve line pen style and width
    static void setLineStyle(QwtPlotCurve* curve, Qt::PenStyle style, qreal width = 0.0);

    // ---- Recommended values ----

    // Recommend a pen width based on the number of data points
    static qreal recommendPenWidth(int pointCount);

    // ---- Force replot ----

    // Force an immediate replot even when autoReplot is disabled
    static void forceReplot(QwtPlot* plot);
};

#endif  // QWT_PLOT_STYLING_H
