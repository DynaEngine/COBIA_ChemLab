/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_ITEM_INFO_H
#define QWT_PLOT_ITEM_INFO_H

#include "qwt_global.h"
#include "qwt_plot_dict.h"

#include <qrect.h>
#include <qset.h>
#include <qstring.h>

class QwtPlot;
class QwtPlotItem;

/**
 * @brief Utility class for querying QwtPlotItem types and filtering plot items
 * @details QwtPlotItemInfo provides static methods to determine the runtime type
 *          of plot items, filter items by category, and retrieve basic information
 *          such as data size and bounding rectangle. This class does not modify
 *          any plot or item state.
 *
 * @code
 * QwtPlot* plot = ...;
 * QwtPlotItemList curves = Qwt::QwtPlotItemInfo::xySeriesItems(plot);
 * for (auto* item : curves) {
 *     qDebug() << item->title().text()
 *              << "has" << Qwt::QwtPlotItemInfo::dataSize(item) << "points";
 * }
 * @endcode
 *
 * @sa QwtPlotDataAccess, QwtPlotStyling
 */
class QWT_EXPORT QwtPlotItemInfo
{
public:
    // Check if item is a QwtPlotSeriesItem subclass
    static bool isSeriesItem(const QwtPlotItem* item);

    // Check if item stores QPointF data (curve, bar chart)
    static bool isXYSeriesItem(const QwtPlotItem* item);

    // Check if item stores QwtIntervalSample data (interval curve, histogram)
    static bool isIntervalSeriesItem(const QwtPlotItem* item);

    // Check if item is a decorator (grid, marker, scale, text label, etc.)
    static bool isDecoratorItem(const QwtPlotItem* item);

    // Check if item holds any data series
    static bool isDataItem(const QwtPlotItem* item);

    // Get all QwtPlotSeriesItem instances from a plot
    static QwtPlotItemList seriesItems(const QwtPlot* plot);

    // Get all QPointF-based series items from a plot
    static QwtPlotItemList xySeriesItems(const QwtPlot* plot);

    // Filter items by a single rtti value
    static QwtPlotItemList filterByRtti(const QwtPlot* plot, int rtti);

    // Filter items by a set of rtti values
    static QwtPlotItemList filterByRtti(const QwtPlot* plot, const QSet< int >& rttiSet);

    // Get all currently visible items
    static QwtPlotItemList visibleItems(const QwtPlot* plot);

    // Get the number of data samples, or -1 if not a data item
    static int dataSize(const QwtPlotItem* item);

    // Get the bounding rectangle of the data, or an invalid rect if not a data item
    static QRectF dataRect(const QwtPlotItem* item);

    // Get a human-readable name for an rtti value
    static QString rttiName(int rtti);
};

#endif  // QWT_PLOT_ITEM_INFO_H
