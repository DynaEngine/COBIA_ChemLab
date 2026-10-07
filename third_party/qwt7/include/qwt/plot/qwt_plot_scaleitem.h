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

#ifndef QWT_PLOT_SCALE_ITEM_H
#define QWT_PLOT_SCALE_ITEM_H

#include "qwt_global.h"
#include "qwt_plot_item.h"
#include "qwt_scale_draw.h"

class QPalette;

/**
 * @brief A class which draws a scale inside the plot canvas
 * @details QwtPlotScaleItem can be used to draw an axis inside the plot canvas.
 *          It might be synchronized to one of the axis of the plot, but can also display its own ticks and labels.
 *
 *          It is allowed to synchronize the scale item with a disabled axis. In plots with vertical and horizontal
 * scale items, it might be necessary to remove ticks at the intersections, by overloading updateScaleDiv().
 *
 *          The scale might be at a specific position (e.g 0.0) or it might be aligned to a canvas border.
 *
 * @par Example
 * The following example shows how to replace the left axis, by a scale item at the x position 0.0.
 * @code
 *   QwtPlotScaleItem *scaleItem = new QwtPlotScaleItem( QwtScaleDraw::RightScale, 0.0 );
 *   scaleItem->setFont( plot->axisWidget( QwtAxis::YLeft )->font() );
 *   scaleItem->attach(plot);
 *
 *   plot->setAxisVisible( QwtAxis::YLeft, false );
 * @endcode
 *
 */
class QWT_EXPORT QwtPlotScaleItem : public QwtPlotItem
{
public:
    // Constructor
    explicit QwtPlotScaleItem(QwtScaleDraw::Alignment = QwtScaleDraw::BottomScale, const double pos = 0.0);

    // Destructor
    ~QwtPlotScaleItem() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Set the scale division
    void setScaleDiv(const QwtScaleDiv&);
    // Get the scale division
    const QwtScaleDiv& scaleDiv() const;

    // Enable/disable scale division from axis
    void setScaleDivFromAxis(bool on);
    // Check if scale division is from axis
    bool isScaleDivFromAxis() const;

    // Set the palette
    void setPalette(const QPalette&);
    // Get the palette
    QPalette palette() const;

    // Set the font
    void setFont(const QFont&);
    // Get the font
    QFont font() const;

    // Set the scale draw
    void setScaleDraw(QwtScaleDraw*);

    // Get the scale draw (const version)
    const QwtScaleDraw* scaleDraw() const;
    // Get the scale draw
    QwtScaleDraw* scaleDraw();

    // Set the position
    void setPosition(double pos);
    // Get the position
    double position() const;

    // Set the border distance
    void setBorderDistance(int);
    // Get the border distance
    int borderDistance() const;

    // Set the alignment
    void setAlignment(QwtScaleDraw::Alignment);

    // Draw the scale item
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

    // Update the scale division
    virtual void updateScaleDiv(const QwtScaleDiv&, const QwtScaleDiv&) override;

private:
    QWT_DECLARE_PRIVATE(QwtPlotScaleItem)
};

#endif
