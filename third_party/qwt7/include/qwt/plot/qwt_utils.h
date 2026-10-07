/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

/**
 * @file qwt_utils.h
 * @brief Qwt plot utility classes collection and compatibility functions
 *
 * This file provides two main features:
 *
 * 1. **Forward includes for plot utility classes**: Including this file gives
 *    access to all Qwt plot utility classes at once. These utility classes
 *    replace the deprecated Qwt::plotItemColor() function with more powerful
 *    and type-safe functionality.
 *
 * 2. **Qt version compatibility function**: qwtExpandedToGlobalStrut() handles
 *    the globalStrut API difference between Qt 5 and Qt 6, primarily used
 *    internally by Qwt widget classes.
 *
 * @section New Plot Utility Classes
 *
 * Since Qwt 7.x, the following 5 dedicated utility classes are recommended
 * for working with plot items:
 *
 * - **QwtPlotFactory**: Factory methods for creating plot items (curves, bars, markers, etc.)
 * - **QwtPlotItemInfo**: Type queries for determining item types and basic information
 * - **QwtPlotDataAccess**: Data access for reading and writing plot item data
 * - **QwtPlotTransform**: Coordinate transformations between pixel and data coordinates
 * - **QwtPlotStyling**: Style operations for getting/setting colors, pens, brushes, etc.
 *
 * All classes are in the Qwt namespace and provide type-safe, rtti-dispatched APIs
 * that are safer and more efficient than the old dynamic_cast approach.
 *
 * @section Usage Example
 *
 * @code
 * #include <qwt_utils.h>
 *
 * // Create a plot
 * QwtPlot* plot = new QwtPlot();
 *
 * // Create a curve using factory method
 * QVector<QPointF> points = {{0,0}, {1,1}, {2,4}};
 * QwtPlotCurve* curve = QwtPlotFactory::createCurve(plot, "Temperature", points);
 *
 * // Query type
 * if (QwtPlotItemInfo::isSeriesItem(item)) {
 *     // Handle series item
 * }
 *
 * // Read data
 * QVector<QPointF> data = QwtPlotDataAccess::xySamples(curve);
 *
 * // Set style
 * QwtPlotStyling::setColor(curve, Qt::blue);
 * QwtPlotStyling::setPenWidth(curve, 2.0);
 *
 * // Coordinate transformation
 * QPointF plotPos = QwtPlotTransform::toPlotCoordinates(plot, screenPos);
 * @endcode
 *
 * @section Migration Guide
 *
 * If you were using Qwt::plotItemColor(), you should now use QwtPlotStyling::color():
 *
 * @code
 * // Old way (deprecated)
 * QColor color = Qwt::plotItemColor(item);
 *
 * // New way (recommended)
 * QColor color = QwtPlotStyling::color(item);
 * @endcode
 *
 * The new approach supports more plot item types and uses rtti dispatch instead of
 * dynamic_cast for better performance.
 *
 * @note The qwtExpandedToGlobalStrut() function is retained for internal Qwt use only.
 *       Application code should not call this function directly.
 */

#ifndef QWT_UTILS_H
#define QWT_UTILS_H

#include "qwt_global.h"

#include <qsize.h>

/**
 * @name Plot Utility Class Forward Includes
 *
 * Including the following headers provides access to all Qwt plot utility classes:
 *
 * - qwt_plot_factory.h: Factory methods
 * - qwt_plot_item_info.h: Type queries
 * - qwt_plot_data_access.h: Data access
 * - qwt_plot_transform.h: Coordinate transformations
 * - qwt_plot_styling.h: Style operations
 *
 * @note If only specific utility classes are needed, include the corresponding
 *       header individually to reduce compilation dependencies.
 */
//!@{
#include "qwt_plot_factory.h"
#include "qwt_plot_item_info.h"
#include "qwt_plot_data_access.h"
#include "qwt_plot_transform.h"
#include "qwt_plot_styling.h"
#include "qwt_pyplot.h"
//!@}

/**
 * @brief Expand a size to the global strut (Qt5/Qt6 compatibility function)
 *
 * In Qt 5.0-5.14, QApplication::globalStrut() returns the application-defined
 * minimum widget size. This function expands the given size to at least that
 * minimum size.
 *
 * In Qt 5.15+ and Qt 6, globalStrut() has been removed and this function
 * returns the original size unchanged.
 *
 * @param size Original size
 * @return Size expanded to the global strut, or the original size on Qt 5.15+/Qt 6
 *
 * @internal
 * @warning This function is for internal use by Qwt widget classes only
 *          (such as QwtDial, QwtKnob, QwtSlider, etc.). Application code
 *          should not call this function.
 *
 * @since Qwt 6.0
 */
QSize QWT_EXPORT qwtExpandedToGlobalStrut(const QSize& size);

#endif  // QWT_UTILS_H
