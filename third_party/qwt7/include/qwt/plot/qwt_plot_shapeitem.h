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

#ifndef QWT_PLOT_SHAPE_ITEM_H
#define QWT_PLOT_SHAPE_ITEM_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

#include <qstring.h>

class QPainterPath;
class QPolygonF;

/**
 * @brief A plot item, which displays any graphical shape that can be defined by a QPainterPath
 * @details QwtPlotShapeItem displays a shape composed from intersecting and uniting
 *          regions, rectangles, ellipses or irregular areas defined by lines, and curves.
 *          The shape is displayed with a pen and brush.
 *
 *          QwtPlotShapeItem offers a couple of optimizations like clipping or weeding.
 *          These algorithms need to convert the painter path into polygons that might be
 *          less performant for paths built from curves and ellipses.
 *
 *          More complex shapes, that can't be expressed by a QPainterPath can be displayed
 *          using QwtPlotGraphicItem.
 *
 * @sa QwtPlotZone, QwtPlotGraphicItem
 *
 */
class QWT_EXPORT QwtPlotShapeItem : public QwtPlotItem
{
public:
    /**
     * @brief Paint attributes
     * @details Attributes to modify the drawing algorithm.
     *          The default disables all attributes
     * @sa setPaintAttribute(), testPaintAttribute()
     *
     */
    enum PaintAttribute
    {
        /**
         * Clip polygons before painting them. In situations, where points
         * are far outside the visible area (f.e when zooming deep) this
         * might be a substantial improvement for the painting performance
         *
         * But polygon clipping will convert the painter path into
         * polygons what might introduce a negative impact on the
         * performance of paths composed from curves or ellipses.
         *
         */
        ClipPolygons = 0x01,
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    /**
     * @brief Legend modes
     * @details Mode how to display the item on the legend
     *
     */
    enum LegendMode
    {
        /// Display a scaled down version of the shape
        LegendShape,

        /// Display a filled rectangle
        LegendColor
    };

    // Constructor
    explicit QwtPlotShapeItem(const QString& title = QString());
    // Constructor with title
    explicit QwtPlotShapeItem(const QwtText& title);

    // Destructor
    ~QwtPlotShapeItem() override;

    // Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set the legend mode
    void setLegendMode(LegendMode);
    // Get the legend mode
    LegendMode legendMode() const;

    // Set the shape as a rectangle
    void setRect(const QRectF&);
    // Set the shape as a polygon
    void setPolygon(const QPolygonF&);

    // Set the shape
    void setShape(const QPainterPath&);
    // Get the shape
    QPainterPath shape() const;

    // Set the pen
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    // Set the pen
    void setPen(const QPen&);
    // Get the pen
    QPen pen() const;

    // Set the brush
    void setBrush(const QBrush&);
    // Get the brush
    QBrush brush() const;

    // Set the render tolerance
    void setRenderTolerance(double);
    // Get the render tolerance
    double renderTolerance() const;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    // Draw the shape item
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

    // Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

    // Get the runtime type information
    virtual int rtti() const override;

private:
    /**
     * @brief Initialize the shape item
     */
    void init();

    QWT_DECLARE_PRIVATE(QwtPlotShapeItem)
};

#endif
