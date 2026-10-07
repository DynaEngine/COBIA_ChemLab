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

#ifndef QWT_PLOT_ZONE_ITEM_H
#define QWT_PLOT_ZONE_ITEM_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

#include <qnamespace.h>
class QwtInterval;
class QPen;
class QBrush;

/**
 * @brief A plot item, which displays a zone
 * @details A horizontal zone highlights an interval of the y axis - a vertical
 *          zone an interval of the x axis - and is unbounded in the opposite direction.
 *          It is filled with a brush and its border lines are optionally displayed with a pen.
 *
 * @note For displaying an area that is bounded for x and y coordinates
 *       use QwtPlotShapeItem
 *
 */
class QWT_EXPORT QwtPlotZoneItem : public QwtPlotItem
{
public:
    // Constructor
    explicit QwtPlotZoneItem();
    // Destructor
    ~QwtPlotZoneItem() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Set the orientation
    void setOrientation(Qt::Orientation);
    // Get the orientation
    Qt::Orientation orientation() const;

    // Set the interval
    void setInterval(double min, double max);
    // Set the interval
    void setInterval(const QwtInterval&);
    // Get the interval
    QwtInterval interval() const;

    // Set the pen
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    // Set the pen
    void setPen(const QPen&);
    // Get the pen
    const QPen& pen() const;

    // Set the brush
    void setBrush(const QBrush&);
    // Get the brush
    const QBrush& brush() const;

    // Draw the zone
    virtual void draw(QPainter*, const QwtScaleMap&, const QwtScaleMap&, const QRectF& canvasRect) const override;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

private:
    QWT_DECLARE_PRIVATE(QwtPlotZoneItem)
};

#endif
