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

#ifndef QWT_PLOT_GRID_H
#define QWT_PLOT_GRID_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

class QPainter;
class QPen;
class QwtScaleMap;
class QwtScaleDiv;

/**
 * @brief A class which draws a coordinate grid
 * @details The QwtPlotGrid class can be used to draw a coordinate grid.
 *          A coordinate grid consists of major and minor vertical
 *          and horizontal grid lines. The locations of the grid lines
 *          are determined by the X and Y scale divisions which can
 *          be assigned with setXDiv() and setYDiv().
 *          The draw() member draws the grid within a bounding
 *          rectangle.
 *
 */

class QWT_EXPORT QwtPlotGrid : public QwtPlotItem
{
public:
    // Constructor
    explicit QwtPlotGrid();

    // Destructor
    ~QwtPlotGrid() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Enable/disable x-axis grid
    void enableX(bool);

    // Check if x-axis grid is enabled
    bool xEnabled() const;

    // Enable/disable y-axis grid
    void enableY(bool);

    // Check if y-axis grid is enabled
    bool yEnabled() const;

    // Enable/disable minor x-axis grid
    void enableXMin(bool);

    // Check if minor x-axis grid is enabled
    bool xMinEnabled() const;

    // Enable/disable minor y-axis grid
    void enableYMin(bool);

    // Check if minor y-axis grid is enabled
    bool yMinEnabled() const;

    // Set x-axis scale division
    void setXDiv(const QwtScaleDiv&);

    // Get x-axis scale division
    const QwtScaleDiv& xScaleDiv() const;

    // Set y-axis scale division
    void setYDiv(const QwtScaleDiv&);

    // Get y-axis scale division
    const QwtScaleDiv& yScaleDiv() const;

    // Set pen for both major and minor grid lines
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);

    // Set pen for both major and minor grid lines
    void setPen(const QPen&);

    // Set pen for major grid lines
    void setMajorPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);

    // Set pen for major grid lines
    void setMajorPen(const QPen&);

    // Get pen for major grid lines
    const QPen& majorPen() const;

    // Set pen for minor grid lines
    void setMinorPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);

    // Set pen for minor grid lines
    void setMinorPen(const QPen&);

    // Get pen for minor grid lines
    const QPen& minorPen() const;

    // Draw the grid
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

    // Update scale divisions
    virtual void updateScaleDiv(const QwtScaleDiv& xScaleDiv, const QwtScaleDiv& yScaleDiv) override;

private:
    /**
     * @brief Draw grid lines
     */
    void drawLines(QPainter*, const QRectF&, Qt::Orientation, const QwtScaleMap&, const QList< double >&) const;

    QWT_DECLARE_PRIVATE(QwtPlotGrid)
};

#endif
