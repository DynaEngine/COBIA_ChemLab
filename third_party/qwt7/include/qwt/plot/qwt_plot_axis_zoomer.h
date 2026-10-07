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

#ifndef QWT_PLOT_AXIS_ZOOMER_H
#define QWT_PLOT_AXIS_ZOOMER_H

#include "qwt_global.h"
#include "qwt_plot_picker.h"

class QSizeF;
template< typename T >
class QStack;

/**
 * @brief QwtPlotAxisZoomer provides stacked zooming for a plot widget
 * @details QwtPlotAxisZoomer selects rectangles from user inputs ( mouse or keyboard )
 *          translates them into plot coordinates and adjusts the axes to them.
 *          The selection is supported by a rubber band and optionally by displaying
 *          the coordinates of the current mouse position.
 *
 *          Zooming can be repeated as often as possible, limited only by
 *          maxStackDepth() or minZoomSize().  Each rectangle is pushed on a stack.
 *
 *          The default setting how to select rectangles is
 *          a QwtPickerDragRectMachine with the following bindings:
 *
 *          - QwtEventPattern::MouseSelect1
 *           The first point of the zoom rectangle is selected by a mouse press,
 *           the second point from the position, where the mouse is released.
 *
 *          - QwtEventPattern::KeySelect1
 *           The first key press selects the first, the second key press
 *           selects the second point.
 *
 *          - QwtEventPattern::KeyAbort
 *           Discard the selection in the state, where the first point
 *           is selected.
 *
 *          To traverse the zoom stack the following bindings are used:
 *
 *          - QwtEventPattern::MouseSelect3, QwtEventPattern::KeyUndo
 *           Zoom out one position on the zoom stack
 *
 *          - QwtEventPattern::MouseSelect6, QwtEventPattern::KeyRedo
 *           Zoom in one position on the zoom stack
 *
 *          - QwtEventPattern::MouseSelect2, QwtEventPattern::KeyHome
 *           Zoom to the zoom base
 *
 *          The setKeyPattern() and setMousePattern() functions can be used
 *          to configure the zoomer actions. The following example
 *          shows, how to configure the 'I' and 'O' keys for zooming in and out
 *          one position on the zoom stack. The "Home" key is used to
 *          "unzoom" the plot.
 *
 *          @code
 *          zoomer = new QwtPlotAxisZoomer( plot );
 *          zoomer->setKeyPattern( QwtEventPattern::KeyRedo, Qt::Key_I, Qt::ShiftModifier );
 *          zoomer->setKeyPattern( QwtEventPattern::KeyUndo, Qt::Key_O, Qt::ShiftModifier );
 *          zoomer->setKeyPattern( QwtEventPattern::KeyHome, Qt::Key_Home );
 *          @endcode
 *
 *          QwtPlotAxisZoomer is tailored for plots with one x and y axis, but it is
 *          allowed to attach a second QwtPlotAxisZoomer ( without rubber band and tracker )
 *          for the other axes.
 *
 * @note The realtime example includes an derived zoomer class that adds
 *       scrollbars to the plot canvas.
 * @sa QwtPlotPanner, QwtPlotMagnifier
 *
 */
class QWT_EXPORT QwtPlotAxisZoomer : public QwtPlotPicker
{
    Q_OBJECT
public:
    // Constructor
    explicit QwtPlotAxisZoomer(QWidget*, bool doReplot = true);
    // Constructor with specified axes
    explicit QwtPlotAxisZoomer(QwtAxisId xAxis, QwtAxisId yAxis, QWidget*, bool doReplot = true);

    // Destructor
    ~QwtPlotAxisZoomer() override;

    // Set zoom base from current scales
    virtual void setZoomBase(bool doReplot = true);
    // Set zoom base from a rectangle
    virtual void setZoomBase(const QRectF&);

    // Get zoom base rectangle
    QRectF zoomBase() const;
    // Get current zoom rectangle
    QRectF zoomRect() const;

    // Set the axes for the zoomer
    virtual void setAxes(QwtAxisId xAxis, QwtAxisId yAxis) override;

    // Set maximum zoom stack depth
    void setMaxStackDepth(int);
    // Get maximum zoom stack depth
    int maxStackDepth() const;

    // Get the zoom stack
    const QStack< QRectF >& zoomStack() const;
    // Set the zoom stack
    void setZoomStack(const QStack< QRectF >&, int zoomRectIndex = -1);

    // Get current zoom rectangle index in stack
    uint zoomRectIndex() const;

public Q_SLOTS:
    // Move zoom rectangle by offset
    void moveBy(double dx, double dy);
    // Move zoom rectangle to position
    virtual void moveTo(const QPointF&);

    // Zoom to a rectangle
    virtual void zoom(const QRectF&);
    // Zoom by offset in stack
    virtual void zoom(int offset);

Q_SIGNALS:
    /**
     * A signal emitting the zoomRect(), when the plot has been
     * zoomed in or out.
     * @param rect Current zoom rectangle.
     *
     */
    void zoomed(const QRectF& rect);

protected:
    /// Rescale the plot
    virtual void rescale();

    /// Get minimum zoom size
    virtual QSizeF minZoomSize() const;

    /// Handle mouse release event
    virtual void widgetMouseReleaseEvent(QMouseEvent*) override;
    /// Handle key press event
    virtual void widgetKeyPressEvent(QKeyEvent*) override;

    /// Begin selection
    virtual void begin() override;
    /// End selection
    virtual bool end(bool ok = true) override;
    /// Accept selection
    virtual bool accept(QPolygon&) const override;

private:
    /// Initialize the zoomer
    void init(bool doReplot);

    QWT_DECLARE_PRIVATE(QwtPlotAxisZoomer)
};

#endif
