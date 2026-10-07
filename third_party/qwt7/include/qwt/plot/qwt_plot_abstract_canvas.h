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

#ifndef QWT_PLOT_ABSTRACT_CANVAS_H
#define QWT_PLOT_ABSTRACT_CANVAS_H

#include "qwt_global.h"
#include <qframe.h>

class QwtPlot;

/**
 * @brief Base class for all type of plot canvases
 */
class QWT_EXPORT QwtPlotAbstractCanvas
{
public:
    /**
     * @brief Focus indicator
     * @details The default setting is NoFocusIndicator
     * @sa setFocusIndicator(), focusIndicator(), drawFocusIndicator()
     */

    enum FocusIndicator
    {
        /**
         * Don't paint a focus indicator
         */
        NoFocusIndicator,

        /**
         * The focus is related to the complete canvas.
         * Paint the focus indicator using drawFocusIndicator()
         */
        CanvasFocusIndicator,

        /**
         * The focus is related to an item (curve, point, ...) on
         * the canvas. It is up to the application to display a
         * focus indication using e.g. highlighting.
         */
        ItemFocusIndicator
    };

    // Constructor
    explicit QwtPlotAbstractCanvas(QWidget* canvasWidget);
    // Destructor
    ~QwtPlotAbstractCanvas();

    // Get the parent plot widget
    QwtPlot* plot();
    // Get the parent plot widget
    const QwtPlot* plot() const;

    // Set the focus indicator
    void setFocusIndicator(FocusIndicator);
    // Get the focus indicator
    FocusIndicator focusIndicator() const;

    // Set the border radius
    void setBorderRadius(double);
    // Get the border radius
    double borderRadius() const;

protected:
    QWidget* canvasWidget();
    const QWidget* canvasWidget() const;

    virtual void drawFocusIndicator(QPainter*);
    virtual void drawBorder(QPainter*);
    virtual void drawBackground(QPainter*);

    void fillBackground(QPainter*);
    void drawCanvas(QPainter*);
    void drawStyled(QPainter*, bool);
    void drawUnstyled(QPainter*);

    QPainterPath canvasBorderPath(const QRect& rect) const;
    void updateStyleSheetInfo();

private:
    QwtPlotAbstractCanvas(const QwtPlotAbstractCanvas&)            = delete;
    QwtPlotAbstractCanvas& operator=(const QwtPlotAbstractCanvas&) = delete;

    QWT_DECLARE_PRIVATE(QwtPlotAbstractCanvas)
};

/**
 * @brief Base class of QwtPlotOpenGLCanvas and QwtPlotGLCanvas
 */
class QWT_EXPORT QwtPlotAbstractGLCanvas : public QwtPlotAbstractCanvas
{
public:
    /**
     * @brief Paint attributes
     * @details The default setting enables BackingStore and Opaque.
     * @sa setPaintAttribute(), testPaintAttribute()
     */
    enum PaintAttribute
    {
        /**
         * @brief Paint double buffered reusing the content of the pixmap buffer when possible
         * @details Using a backing store might improve the performance significantly,
         *          when working with widget overlays (like rubber bands).
         *          Disabling the cache might improve the performance for incremental paints
         *          (using QwtPlotDirectPainter).
         * @sa backingStore(), invalidateBackingStore()
         */
        BackingStore = 1,

        /**
         * @brief When ImmediatePaint is set replot() calls repaint() instead of update()
         * @sa replot(), QWidget::repaint(), QWidget::update()
         */
        ImmediatePaint = 8,
    };

    //! Paint attributes
    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    // Constructor
    explicit QwtPlotAbstractGLCanvas(QWidget* canvasWidget);
    // Destructor
    ~QwtPlotAbstractGLCanvas();

    // Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set the frame style
    void setFrameStyle(int style);
    // Get the frame style
    int frameStyle() const;

    // Set the frame shadow
    void setFrameShadow(QFrame::Shadow);
    // Get the frame shadow
    QFrame::Shadow frameShadow() const;

    // Set the frame shape
    void setFrameShape(QFrame::Shape);
    // Get the frame shape
    QFrame::Shape frameShape() const;

    // Set the line width
    void setLineWidth(int);
    // Get the line width
    int lineWidth() const;

    // Set the mid line width
    void setMidLineWidth(int);
    // Get the mid line width
    int midLineWidth() const;

    // Get the frame width
    int frameWidth() const;
    // Get the frame rect
    QRect frameRect() const;

    //! Invalidate the internal backing store
    virtual void invalidateBackingStore() = 0;

protected:
    void replot();
    void draw(QPainter*);

private:
    virtual void clearBackingStore() = 0;

    QWT_DECLARE_PRIVATE(QwtPlotAbstractGLCanvas)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotAbstractGLCanvas::PaintAttributes)

#endif
