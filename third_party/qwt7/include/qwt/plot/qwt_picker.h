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
 *        - QwtPanner -> QwtCachePanner (pixmap-cache version)
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

#ifndef QWT_PICKER
#define QWT_PICKER

#include "qwt_global.h"
#include "qwt_event_pattern.h"

#include <qobject.h>
#include <QPen>
#include <QFont>
#include <QPainterPath>
#include <QPoint>
#include <QPolygon>
#include <QSize>
#include <QRect>
#include <QRegion>
class QwtPickerMachine;
class QwtWidgetOverlay;
class QwtText;
class QWidget;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QPainter;

/**
 * @brief QwtPicker provides selections on a widget
 * @details QwtPicker filters all enter, leave, mouse and keyboard events of a widget
 *          and translates them into an array of selected points.
 *
 *          The way how the points are collected depends on type of state machine
 *          that is connected to the picker. Qwt offers a couple of predefined
 *          state machines for selecting:
 *
 *          - Nothing: QwtPickerTrackerMachine
 *          - Single points: QwtPickerClickPointMachine, QwtPickerDragPointMachine
 *          - Rectangles: QwtPickerClickRectMachine, QwtPickerDragRectMachine
 *          - Polygons: QwtPickerPolygonMachine
 *
 *          While these state machines cover the most common ways to collect points
 *          it is also possible to implement individual machines as well.
 *
 *          QwtPicker translates the picked points into a selection using the
 *          adjustedPoints() method. adjustedPoints() is intended to be reimplemented
 *          to fix up the selection according to application specific requirements.
 *          (F.e. when an application accepts rectangles of a fixed aspect ratio only.)
 *
 *          Optionally QwtPicker support the process of collecting points by a
 *          rubber band and tracker displaying a text for the current mouse
 *          position.
 *
 *          The state machine triggers the following commands:
 *          - begin(): Activate/Initialize the selection.
 *          - append(): Add a new point.
 *          - move(): Change the position of the last point.
 *          - remove(): Remove the last point.
 *          - end(): Terminate the selection and call accept to validate the picked points.
 *
 *          The picker is active (isActive()), between begin() and end().
 *          In active state the rubber band is displayed, and the tracker is visible
 *          in case of trackerMode is ActiveOnly or AlwaysOn.
 *
 *          The cursor can be moved using the arrow keys. All selections can be aborted
 *          using the abort key. (QwtEventPattern::KeyPatternCode)
 *
 * @warning In case of QWidget::NoFocus the focus policy of the observed
 *          widget is set to QWidget::WheelFocus and mouse tracking
 *          will be manipulated while the picker is active,
 *          or if trackerMode() is AlwaysOn.
 *
 * @par Example
 * @code
 * #include <qwt_picker.h>
 * #include <qwt_picker_machine.h>
 *
 * QwtPicker *picker = new QwtPicker(widget);
 * picker->setStateMachine(new QwtPickerDragRectMachine);
 * picker->setTrackerMode(QwtPicker::ActiveOnly);
 * picker->setRubberBand(QwtPicker::RectRubberBand);
 * @endcode
 */
class QWT_EXPORT QwtPicker : public QObject, public QwtEventPattern
{
    Q_OBJECT

    Q_ENUMS(RubberBand DisplayMode ResizeMode)

    Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)

    Q_PROPERTY(DisplayMode trackerMode READ trackerMode WRITE setTrackerMode)
    Q_PROPERTY(QPen trackerPen READ trackerPen WRITE setTrackerPen)
    Q_PROPERTY(QFont trackerFont READ trackerFont WRITE setTrackerFont)

    Q_PROPERTY(RubberBand rubberBand READ rubberBand WRITE setRubberBand)
    Q_PROPERTY(QPen rubberBandPen READ rubberBandPen WRITE setRubberBandPen)
    QWT_DECLARE_PRIVATE(QwtPicker)
public:
    /**
     * @brief Rubber band style
     * @details The default value is QwtPicker::NoRubberBand.
     * @sa setRubberBand(), rubberBand()
     */
    enum RubberBand
    {
        //! No rubberband.
        NoRubberBand = 0,

        //! A horizontal line ( only for QwtPickerMachine::PointSelection )
        HLineRubberBand,

        //! A vertical line ( only for QwtPickerMachine::PointSelection )
        VLineRubberBand,

        //! A crosshair ( only for QwtPickerMachine::PointSelection )
        CrossRubberBand,

        //! A rectangle ( only for QwtPickerMachine::RectSelection )
        RectRubberBand,

        //! An ellipse ( only for QwtPickerMachine::RectSelection )
        EllipseRubberBand,

        //! A polygon ( only for QwtPickerMachine::PolygonSelection )
        PolygonRubberBand,

        /**
         * @brief Values >= UserRubberBand can be used to define additional rubber bands
         */
        UserRubberBand = 100
    };

    /**
     * @brief Display mode
     * @sa setTrackerMode(), trackerMode(), isActive()
     */
    enum DisplayMode
    {
        //! Display never
        AlwaysOff,

        //! Display always
        AlwaysOn,

        //! Display only when the selection is active
        ActiveOnly
    };

    /**
     * @brief Controls what to do with the selected points when the observed widget is resized
     * @details The default value is QwtPicker::Stretch.
     * @sa setResizeMode()
     */
    enum ResizeMode
    {
        //! All points are scaled according to the new size,
        Stretch,

        //! All points remain unchanged.
        KeepSize
    };

    // Constructor
    explicit QwtPicker(QWidget* parent);
    // Constructor with rubber band and tracker mode
    explicit QwtPicker(RubberBand rubberBand, DisplayMode trackerMode, QWidget*);

    // Destructor
    ~QwtPicker() override;

    // Set the state machine
    void setStateMachine(QwtPickerMachine*);
    // Return the state machine (const)
    const QwtPickerMachine* stateMachine() const;
    // Return the state machine
    QwtPickerMachine* stateMachine();

    // Set the rubber band style
    void setRubberBand(RubberBand);
    // Return the rubber band style
    RubberBand rubberBand() const;

    // Set the tracker display mode
    void setTrackerMode(DisplayMode);
    // Return the tracker display mode
    DisplayMode trackerMode() const;

    // Set the resize mode
    void setResizeMode(ResizeMode);
    // Return the resize mode
    ResizeMode resizeMode() const;

    // Set the rubber band pen
    void setRubberBandPen(const QPen&);
    // Return the rubber band pen
    QPen rubberBandPen() const;

    // Set the tracker pen
    void setTrackerPen(const QPen&);
    // Return the tracker pen
    QPen trackerPen() const;

    // Set the tracker font
    void setTrackerFont(const QFont&);
    // Return the tracker font
    QFont trackerFont() const;

    // Return true when enabled
    bool isEnabled() const;
    // Return true if the selection is active
    bool isActive() const;

    // Event filter for handling events
    virtual bool eventFilter(QObject*, QEvent*) override;

    // Return the parent widget
    QWidget* parentWidget();
    // Return the parent widget (const)
    const QWidget* parentWidget() const;

    // Return the pick area
    virtual QPainterPath pickArea() const;

    // Draw the rubber band
    virtual void drawRubberBand(QPainter*) const;
    // Draw the tracker
    virtual void drawTracker(QPainter*) const;

    // Return the tracker mask
    virtual QRegion trackerMask() const;
    // Return the rubber band mask
    virtual QRegion rubberBandMask() const;

    // Return the tracker text for a position
    virtual QwtText trackerText(const QPoint& pos) const;
    // Return the tracker rectangle
    virtual QRect trackerRect(const QFont&) const;
    // Set the tracker position manually (for displaying picker without mouse)
    virtual void setTrackerPosition(const QPoint& pos);
    // Return the tracker position
    QPoint trackerPosition() const;
    // Return the selected points
    QPolygon selection() const;
    // Update the display
    void update();
    // Set the picker active manually
    void setActive(bool on);
public Q_SLOTS:
    // Enable or disable the picker
    void setEnabled(bool);

Q_SIGNALS:
    /**
     * @brief Signal indicating when the picker has been activated
     * @details Together with setEnabled() it can be used to implement
     *          selections with more than one picker.
     * @param on True, when the picker has been activated
     */
    void activated(bool on);

    /**
     * @brief Signal emitting the selected points at the end of a selection
     * @param polygon Selected points
     */
    void selected(const QPolygon& polygon);

    /**
     * @brief Signal emitted when a point has been appended to the selection
     * @param pos Position of the appended point
     * @sa append(), moved()
     */
    void appended(const QPoint& pos);

    /**
     * @brief Signal emitted whenever the last appended point of the selection has been moved
     * @param pos Position of the moved last point of the selection
     * @sa move(), appended()
     */
    void moved(const QPoint& pos);

    /**
     * @brief Signal emitted whenever the last appended point of the selection has been removed
     * @param pos Position of the point that has been removed
     * @sa remove(), appended()
     */
    void removed(const QPoint& pos);

    /**
     * @brief Signal emitted when the active selection has been changed
     * @details This might happen when the observed widget is resized.
     * @param selection Changed selection
     * @sa stretchSelection()
     */
    void changed(const QPolygon& selection);

protected:
    virtual QPolygon adjustedPoints(const QPolygon&) const;

    virtual void transition(const QEvent*);

    virtual void begin();
    virtual void append(const QPoint&);
    virtual void move(const QPoint&);
    virtual void remove();
    virtual bool end(bool ok = true);

    virtual bool accept(QPolygon&) const;
    virtual void reset();

    virtual void widgetMousePressEvent(QMouseEvent*);
    virtual void widgetMouseReleaseEvent(QMouseEvent*);
    virtual void widgetMouseDoubleClickEvent(QMouseEvent*);
    virtual void widgetMouseMoveEvent(QMouseEvent*);
    virtual void widgetWheelEvent(QWheelEvent*);
    virtual void widgetKeyPressEvent(QKeyEvent*);
    virtual void widgetKeyReleaseEvent(QKeyEvent*);
    virtual void widgetEnterEvent(QEvent*);
    virtual void widgetLeaveEvent(QEvent*);

    virtual void stretchSelection(const QSize& oldSize, const QSize& newSize);

    virtual void updateDisplay();

    const QwtWidgetOverlay* rubberBandOverlay() const;
    const QwtWidgetOverlay* trackerOverlay() const;

    const QPolygon& pickedPoints() const;

private:
    void init(QWidget*, RubberBand rubberBand, DisplayMode trackerMode);

    void setMouseTracking(bool);
};

#endif
