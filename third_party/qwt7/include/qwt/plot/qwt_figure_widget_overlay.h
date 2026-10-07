#ifndef QWTFIGUREWIDGETOVERLAY_H
#define QWTFIGUREWIDGETOVERLAY_H
#include "qwt_widget_overlay.h"
// Qt
class QEvent;
class QMouseEvent;
class QHoverEvent;
class QKeyEvent;
// Qwt
class QwtFigure;
class QwtPlot;

/**
 * @brief An overlay widget for QwtFigure that provides interactive manipulation
 *
 * This class provides functionality for resizing child widgets within a figure window
 * and changing the currently active plot. You can extend this class to implement
 * additional operations.
 *
 * @note QwtFigureWidgetOverlay does not directly modify sizes. Size changes are
 *       performed in the managing window, allowing greater flexibility such as
 *       implementing undo/redo functionality.
 */
class QWT_EXPORT QwtFigureWidgetOverlay : public QwtWidgetOverlay
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtFigureWidgetOverlay)
public:
    /**
     * @brief Enum for marking rectangular control regions
     */
    enum ControlType
    {
        ControlLineTop,
        ControlLineBottom,
        ControlLineLeft,
        ControlLineRight,
        ControlPointTopLeft,
        ControlPointTopRight,
        ControlPointBottomLeft,
        ControlPointBottomRight,
        Inner,
        OutSide
    };
    Q_ENUM(ControlType)

    /**
     * @brief Built-in functionality flags
     */
    enum BuiltInFunctionsFlag
    {
        FunSelectCurrentPlot = 1,  ///< Enable changing the currently selected plot
        FunResizePlot        = 2   ///< Enable resizing plots
    };
    Q_ENUM(BuiltInFunctionsFlag)
    Q_DECLARE_FLAGS(BuiltInFunctions, BuiltInFunctionsFlag)
    Q_FLAG(BuiltInFunctions)

public:
    /// Constructor, passing nullptr is not allowed
    explicit QwtFigureWidgetOverlay(QwtFigure* fig);
    ~QwtFigureWidgetOverlay() override;
    /// Returns the associated QwtFigure
    QwtFigure* figure() const;
    void setTransparentForMouseEvents(bool on);
    bool isTransparentForMouseEvents() const;

public:
    /// Returns the cursor shape based on control type
    static Qt::CursorShape controlTypeToCursor(ControlType rr);
    /// Determines control type based on point position relative to rectangle
    static ControlType getPositionControlType(const QPoint& pos, const QRect& region, int err = 1);
    /// Checks if a point is on the edge of a rectangle
    static bool isPointInRectEdget(const QPoint& pos, const QRect& region, int err = 1);
    /// Enables or disables built-in functions
    void setBuiltInFunctionsEnable(BuiltInFunctionsFlag flag, bool on = true);
    /// Tests if a built-in function is enabled
    bool testBuiltInFunctions(BuiltInFunctionsFlag flag) const;
    /// Checks if there is an active widget
    bool hasActiveWidget() const;
    /// Checks if currently resizing
    bool isResizing() const;
    /// Sets the border pen
    void setBorderPen(const QPen& p);
    /// Returns the border pen
    QPen borderPen() const;
    /// Sets the control point brush
    void setControlPointBrush(const QBrush& b);
    /// Returns the control point brush
    QBrush controlPointBrush() const;
    /// Sets the control point size
    void setControlPointSize(const QSize& c);
    /// Returns the control point size (default 8x8)
    QSize controlPointSize() const;
    /// Selects the next widget as the active widget
    void selectNextWidget(bool forward = true);
    /// Selects the next plot as the active widget
    void selectNextPlot(bool forward = true);
    /// Returns the current active widget
    QWidget* currentActiveWidget() const;
    /// Returns the current active plot
    QwtPlot* currentActivePlot() const;
    /// Shows or hides percentage text
    void showPercentText(bool on = true);
    /// Cancels the operation, emits finished(false) signal. Override should call this explicitly
    virtual bool cancel();
public Q_SLOTS:
    /// Changes the active widget
    void setActiveWidget(QWidget* w);

protected:
    virtual void drawOverlay(QPainter* p) const override;
    virtual QRegion maskHint() const override;
    /// Draws the active widget
    virtual void drawActiveWidget(QPainter* painter, QWidget* activeW) const;
    /// Draws the resizing rubber-band control line
    virtual void drawResizeingControlLine(QPainter* painter, const QRectF& willSetNormRect) const;
    /// Draws the control line
    virtual void drawControlLine(QPainter* painter, const QRect& actualRect, const QRectF& normRect) const;
    /// Helper function to mark the start of resizing
    void startResize(ControlType controlType, const QPoint& pos);

protected:
    void mouseMoveEvent(QMouseEvent* me) override;
    void mouseReleaseEvent(QMouseEvent* me) override;
    void mousePressEvent(QMouseEvent* me) override;
    void keyPressEvent(QKeyEvent* ke) override;
Q_SIGNALS:

    /**
     * @brief Signal emitted when widget normalized geometry changes
     * @param w The widget
     * @param oldNormGeo The old normalized geometry
     * @param newNormGeo The new normalized geometry
     */
    void widgetNormGeometryChanged(QWidget* w, const QRectF& oldNormGeo, const QRectF& newNormGeo);

    /**
     * @brief Signal emitted when the active widget changes
     * @param oldActive The previously active widget (may be nullptr if none)
     * @param newActive The newly active widget (may be nullptr if none)
     */
    void activeWidgetChanged(QWidget* oldActive, QWidget* newActive);

    /**
     * @brief Signal emitted when the operation finishes
     * @param isCancel Whether the operation was cancelled
     */
    void finished(bool isCancel);
private Q_SLOTS:
    void onAxesRemove(QwtPlot* removedAxes);
};

#endif  // QWTFIGUREWIDGETOVERLAY_H