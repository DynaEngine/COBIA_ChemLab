/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 2024   ChenZongYan <czy.t@163.com>
 *****************************************************************************/
#ifndef QWT_PLOT_CANVAS_ZOOMER_H
#define QWT_PLOT_CANVAS_ZOOMER_H
#include "qwt_global.h"
#include "qwt_canvas_picker.h"
#include "qwt_axis_id.h"
#include "qwt_interval.h"
#include <QPointer>
// Qt
class QSizeF;
template< typename T >
class QStack;
// Qwt
class QwtPlot;

/**
 * @brief Struct that stores zoom states for all four axes
 */
struct QWT_EXPORT QwtPlotCanvasZoomState
{
public:
    // Default constructor
    QwtPlotCanvasZoomState();

    // Constructor with plot and axis intervals
    QwtPlotCanvasZoomState(QwtPlot* p,
                           const QwtInterval& yLeft,
                           const QwtInterval& yRight,
                           const QwtInterval& xBottom,
                           const QwtInterval& xTop);

    // Create zoom state from current plot axis ranges
    static QwtPlotCanvasZoomState fromPlot(QwtPlot* plot);

    // Apply this zoom state to the associated plot
    void apply() const;

    // Check if two zoom states are equal
    bool operator==(const QwtPlotCanvasZoomState& other) const;

    // Check if two zoom states are not equal
    bool operator!=(const QwtPlotCanvasZoomState& other) const;

    // Check if this zoom state is valid (has an associated plot)
    bool isValid() const;

public:
    QPointer< QwtPlot > plot;
    QwtInterval axisInterval[ QwtAxis::AxisPositions ];
};
Q_DECLARE_METATYPE(QwtPlotCanvasZoomState)

/**
 * @brief QwtPlotCanvasZoomer provides zooming for all axes of a plot canvas
 * @details QwtPlotCanvasZoomer selects rectangles from user inputs (mouse or keyboard)
 *          and adjusts ALL axes of the plot simultaneously. Unlike QwtPlotZoomer which
 *          only works on two axes, this zoomer works on the entire canvas and maintains
 *          separate ranges for all four axes.
 *
 *          The selection is supported by a rubber band and optionally by displaying
 *          the coordinates of the current mouse position.
 *
 *          Zooming can be repeated as often as possible, limited only by
 *          maxStackDepth() or minZoomSize(). Each zoom state is pushed on a stack.
 *
 *          Mouse shortcuts:
 *          - MouseSelect2: Reset to base
 *          - MouseSelect3: Zoom out (back)
 *          - MouseSelect6: Zoom in (forward)
 *
 *          Keyboard shortcuts:
 *          - KeyUndo: Zoom out (back)
 *          - KeyRedo: Zoom in (forward)
 *          - KeyHome: Reset to base
 *
 * @sa QwtPlotPanner, QwtPlotMagnifier
 */
class QWT_EXPORT QwtPlotCanvasZoomer : public QwtCanvasPicker
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtPlotCanvasZoomer)
public:
    // Constructor
    explicit QwtPlotCanvasZoomer(QWidget* canvas, bool doReplot = true);

    // Destructor
    ~QwtPlotCanvasZoomer() override;

    // Set zoom base to current axis ranges, min zoom size is calculated based on this
    virtual void setZoomBase(bool doReplot = true);

    // Enable or disable automatic replot after zoom operations
    void setAutoReplot(bool on = true);

    // Check if automatic replot is enabled
    bool isAutoReplot() const;

    // Get the base zoom state (initial unzoomed state)
    QList< QwtPlotCanvasZoomState > zoomBase() const;

    // Get the current zoom state
    QList< QwtPlotCanvasZoomState > zoomState() const;

    // Set maximum number of zoom levels in the stack, -1 for unlimited
    void setMaxStackDepth(int);

    // Get maximum stack depth
    int maxStackDepth() const;

    // Get the zoom stack containing all zoom states
    const QStack< QList< QwtPlotCanvasZoomState > >& zoomStack() const;

    // Get the current zoom state index in the stack
    uint zoomStateIndex() const;

public Q_SLOTS:
    // Navigate in the zoom stack by offset (negative = zoom out, positive = zoom in)
    virtual void zoom(int offset);

    // Append a new zoom state to the stack
    virtual void appendZoom(const QList< QwtPlotCanvasZoomState >& rect);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the plot has been zoomed in or out
     * @param state Current zoom state containing all axis ranges
     */
    void zoomed(const QList< QwtPlotCanvasZoomState >& state);

protected:
    virtual void rescale();

    virtual void widgetMouseReleaseEvent(QMouseEvent*) override;
    virtual void widgetKeyPressEvent(QKeyEvent*) override;

    virtual void begin() override;
    virtual bool end(bool ok = true) override;
    virtual bool accept(QPolygon&) const override;

private:
    void init(bool doReplot);

    QList< QwtPlotCanvasZoomState > canvasRectToZoomStateList(const QRect& pixelRect) const;
    QwtPlotCanvasZoomState canvasRectToZoomState(QwtPlot* plt, const QRect& pixelRect) const;
    void moveCurrentState(double dx, double dy);
};

#endif  // QWTCANVASZOOMER_H
