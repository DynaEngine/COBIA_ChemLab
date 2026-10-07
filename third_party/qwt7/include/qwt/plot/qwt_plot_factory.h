/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_FACTORY_H
#define QWT_PLOT_FACTORY_H

#include "qwt_global.h"
#include "qwt_axis_id.h"

#include <qbrush.h>
#include <qpen.h>
#include <qvector.h>

class QString;
class QPointF;
class QRectF;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotBarChart;
class QwtPlotMultiBarChart;
class QwtPlotHistogram;
class QwtPlotIntervalCurve;
class QwtPlotTradingCurve;
class QwtPlotSpectroCurve;
class QwtPlotVectorField;
class QwtPlotBoxChart;
class QwtPlotGrid;
class QwtPlotMarker;
class QwtPlotZoneItem;
class QwtPlotArrowMarker;
class QwtPlotGraphicItem;
class QwtPlotTextLabel;
class QwtPlotLegendItem;
class QwtPlotScaleItem;
class QwtGraphic;
class QwtInterval;
class QwtIntervalSample;
class QwtPoint3D;
class QwtOHLCSample;
class QwtSetSample;
class QwtVectorFieldSample;
class QwtBoxSample;

/**
 * @brief Factory class for creating and attaching QwtPlotItem instances to a QwtPlot
 * @details QwtPlotFactory provides static methods to create various plot items with
 *          common default settings, set their data, and attach them to a plot in
 *          a single call. This simplifies the common pattern of:
 *          create item -> configure -> set data -> attach.
 *
 *          All factory methods return the newly created item, which is already
 *          attached to the plot. The caller owns the item and may further
 *          customize it.
 *
 * @code
 * QwtPlot* plot = new QwtPlot();
 *
 * // Create a curve with data in one line
 * QVector<QPointF> data = {{0, 1}, {1, 3}, {2, 2}, {3, 5}};
 * QwtPlotCurve* curve = Qwt::QwtPlotFactory::createCurve(plot, "My Curve", data);
 *
 * // Add a grid
 * Qwt::QwtPlotFactory::createGrid(plot);
 *
 * // Add a horizontal marker line
 * Qwt::QwtPlotFactory::createHLine(plot, 2.5);
 * @endcode
 *
 * @sa QwtPlotDataAccess, QwtPlotStyling
 */
class QWT_EXPORT QwtPlotFactory
{
public:
    // ---- Curve ----

    // Create a curve from QPointF data
    static QwtPlotCurve* createCurve(QwtPlot* plot,
                                     const QString& title,
                                     const QVector< QPointF >& data,
                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // Create a curve from separate x and y vectors
    static QwtPlotCurve* createCurve(QwtPlot* plot,
                                     const QString& title,
                                     const QVector< double >& x,
                                     const QVector< double >& y,
                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // Create a curve from y-only data (x = index)
    static QwtPlotCurve* createCurve(QwtPlot* plot,
                                     const QString& title,
                                     const QVector< double >& y,
                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Bar chart ----

    // Create a bar chart from y-only values
    static QwtPlotBarChart* createBarChart(QwtPlot* plot,
                                           const QString& title,
                                           const QVector< double >& values,
                                           QwtAxisId xAxis = QwtAxis::XBottom,
                                           QwtAxisId yAxis = QwtAxis::YLeft);

    // Create a bar chart from QPointF data
    static QwtPlotBarChart* createBarChart(QwtPlot* plot,
                                           const QString& title,
                                           const QVector< QPointF >& data,
                                           QwtAxisId xAxis = QwtAxis::XBottom,
                                           QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Multi bar chart ----

    // Create a multi bar chart from QwtSetSample data
    static QwtPlotMultiBarChart* createMultiBarChart(QwtPlot* plot,
                                                     const QString& title,
                                                     const QVector< QwtSetSample >& data,
                                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // Create a multi bar chart from nested vectors
    static QwtPlotMultiBarChart* createMultiBarChart(QwtPlot* plot,
                                                     const QString& title,
                                                     const QVector< QVector< double > >& data,
                                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Histogram ----

    // Create a histogram from interval samples
    static QwtPlotHistogram* createHistogram(QwtPlot* plot,
                                             const QString& title,
                                             const QVector< QwtIntervalSample >& data,
                                             QwtAxisId xAxis = QwtAxis::XBottom,
                                             QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Interval curve ----

    // Create an interval curve
    static QwtPlotIntervalCurve* createIntervalCurve(QwtPlot* plot,
                                                     const QString& title,
                                                     const QVector< QwtIntervalSample >& data,
                                                     QwtAxisId xAxis = QwtAxis::XBottom,
                                                     QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Trading curve (K-line / OHLC) ----

    // Create a trading curve from OHLC samples
    static QwtPlotTradingCurve* createTradingCurve(QwtPlot* plot,
                                                   const QString& title,
                                                   const QVector< QwtOHLCSample >& data,
                                                   QwtAxisId xAxis = QwtAxis::XBottom,
                                                   QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Spectro curve (3D) ----

    // Create a spectro curve from 3D point data
    static QwtPlotSpectroCurve* createSpectroCurve(QwtPlot* plot,
                                                   const QString& title,
                                                   const QVector< QwtPoint3D >& data,
                                                   QwtAxisId xAxis = QwtAxis::XBottom,
                                                   QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Vector field ----

    // Create a vector field from samples
    static QwtPlotVectorField* createVectorField(QwtPlot* plot,
                                                 const QString& title,
                                                 const QVector< QwtVectorFieldSample >& data,
                                                 QwtAxisId xAxis = QwtAxis::XBottom,
                                                 QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Box chart ----

    // Create a box chart from box samples
    static QwtPlotBoxChart* createBoxChart(QwtPlot* plot,
                                           const QString& title,
                                           const QVector< QwtBoxSample >& data,
                                           QwtAxisId xAxis = QwtAxis::XBottom,
                                           QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Grid ----

    // Create and attach a grid
    static QwtPlotGrid* createGrid(QwtPlot* plot,
                                   bool enableMinor     = false,
                                   const QPen& majorPen = QPen(Qt::gray, 0, Qt::DotLine),
                                   const QPen& minorPen = QPen(Qt::lightGray, 0, Qt::DotLine));

    // ---- Marker ----

    // Create a point marker with optional label
    static QwtPlotMarker* createMarker(QwtPlot* plot,
                                       const QPointF& pos,
                                       const QString& label = QString(),
                                       QwtAxisId xAxis      = QwtAxis::XBottom,
                                       QwtAxisId yAxis      = QwtAxis::YLeft);

    // Create a horizontal line marker
    static QwtPlotMarker* createHLine(QwtPlot* plot, double y, const QPen& pen = QPen(Qt::gray, 0, Qt::DashLine));

    // Create a vertical line marker
    static QwtPlotMarker* createVLine(QwtPlot* plot, double x, const QPen& pen = QPen(Qt::gray, 0, Qt::DashLine));

    // ---- Zone ----

    // Create a highlighted zone
    static QwtPlotZoneItem* createZone(QwtPlot* plot,
                                       const QwtInterval& interval,
                                       Qt::Orientation orientation = Qt::Vertical,
                                       const QBrush& brush         = QBrush(QColor(0, 0, 255, 30)));

    // ---- Arrow marker ----

    // Create an arrow marker
    static QwtPlotArrowMarker* createArrowMarker(QwtPlot* plot,
                                                 const QPointF& start,
                                                 const QPointF& end,
                                                 QwtAxisId xAxis = QwtAxis::XBottom,
                                                 QwtAxisId yAxis = QwtAxis::YLeft);

    // ---- Graphic ----

    // Create a graphic item
    static QwtPlotGraphicItem* createGraphic(QwtPlot* plot, const QRectF& rect, const QwtGraphic& graphic);

    // ---- Text label ----

    // Create a text label on the canvas
    static QwtPlotTextLabel*
    createTextLabel(QwtPlot* plot, const QString& text, Qt::Alignment alignment = Qt::AlignTop | Qt::AlignHCenter);

    // ---- Legend ----

    // Create an in-canvas legend item
    static QwtPlotLegendItem* createLegend(QwtPlot* plot);

    // ---- Scale item ----

    // Create a scale item at a specific axis
    static QwtPlotScaleItem* createScaleItem(QwtPlot* plot, QwtAxisId axis);
};

#endif  // QWT_PLOT_FACTORY_H
