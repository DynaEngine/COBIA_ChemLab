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

#ifndef QWT_WHEEL_H
#define QWT_WHEEL_H

#include "qwt_global.h"
#include <qwidget.h>

/**
 * @brief The Wheel Widget
 * @details The wheel widget can be used to change values over a very large range
 *          in very small steps. Using the setMass() member, it can be configured
 *          as a flying wheel.
 *          The default range of the wheel is [0.0, 100.0]
 * @sa The radio example.
 */
class QWT_EXPORT QwtWheel : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged USER true)

    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)

    Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int pageStepCount READ pageStepCount WRITE setPageStepCount)
    Q_PROPERTY(bool stepAlignment READ stepAlignment WRITE setStepAlignment)

    Q_PROPERTY(bool tracking READ isTracking WRITE setTracking)
    Q_PROPERTY(bool wrapping READ wrapping WRITE setWrapping)
    Q_PROPERTY(bool inverted READ isInverted WRITE setInverted)

    Q_PROPERTY(double mass READ mass WRITE setMass)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

    Q_PROPERTY(double totalAngle READ totalAngle WRITE setTotalAngle)
    Q_PROPERTY(double viewAngle READ viewAngle WRITE setViewAngle)
    Q_PROPERTY(int tickCount READ tickCount WRITE setTickCount)
    Q_PROPERTY(int wheelWidth READ wheelWidth WRITE setWheelWidth)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(int wheelBorderWidth READ wheelBorderWidth WRITE setWheelBorderWidth)
    Q_PROPERTY(bool flatStyle READ flatStyle WRITE setFlatStyle)

public:
    /// Constructor
    explicit QwtWheel(QWidget* parent = nullptr);
    /// Destructor
    ~QwtWheel() override;

    /// Return the current value
    double value() const;

    /// Set the wheel orientation
    void setOrientation(Qt::Orientation);
    /// Return the orientation
    Qt::Orientation orientation() const;

    /// Return the total angle
    double totalAngle() const;
    /// Return the view angle
    double viewAngle() const;

    /// Set the number of ticks
    void setTickCount(int);
    /// Return the number of ticks
    int tickCount() const;

    /// Set the wheel width
    void setWheelWidth(int);
    /// Return the wheel width
    int wheelWidth() const;

    /// Set the wheel border width
    void setWheelBorderWidth(int);
    /// Return the wheel border width
    int wheelBorderWidth() const;

    /// Set the outer border width
    void setBorderWidth(int);
    /// Return the outer border width
    int borderWidth() const;

    /// Set flat style
    void setFlatStyle(bool);
    /// Return flat style
    bool flatStyle() const;

    /// Set inverted appearance
    void setInverted(bool);
    /// Return whether the wheel is inverted
    bool isInverted() const;

    /// Set wrapping mode
    void setWrapping(bool);
    /// Return whether wrapping is enabled
    bool wrapping() const;

    /// Set single step size
    void setSingleStep(double);
    /// Return single step size
    double singleStep() const;

    /// Set page step count
    void setPageStepCount(int);
    /// Return page step count
    int pageStepCount() const;

    /// Set step alignment
    void setStepAlignment(bool on);
    /// Return whether step alignment is enabled
    bool stepAlignment() const;

    /// Set value range
    void setRange(double min, double max);

    /// Set minimum value
    void setMinimum(double);
    /// Return minimum value
    double minimum() const;

    /// Set maximum value
    void setMaximum(double);
    /// Return maximum value
    double maximum() const;

    /// Set update interval
    void setUpdateInterval(int);
    /// Return update interval
    int updateInterval() const;

    /// Set tracking mode
    void setTracking(bool);
    /// Return whether tracking is enabled
    bool isTracking() const;

    /// Return mass for flywheel effect
    double mass() const;

public Q_SLOTS:
    void setValue(double);
    void setTotalAngle(double);
    void setViewAngle(double);
    void setMass(double);

Q_SIGNALS:

    /**
     * @brief Notify a change of value
     * @details When tracking is enabled this signal will be emitted every
     *          time the value changes.
     * @param[in] value New value
     * @sa setTracking()
     */
    void valueChanged(double value);

    /**
     * @brief Signal emitted when the user presses the wheel with the mouse
     */
    void wheelPressed();

    /**
     * @brief Signal emitted when the user releases the mouse
     */
    void wheelReleased();

    /**
     * @brief Signal emitted when the user moves the wheel with the mouse
     * @param[in] value New value
     */
    void wheelMoved(double value);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void keyPressEvent(QKeyEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void timerEvent(QTimerEvent*) override;

    void stopFlying();

    QRect wheelRect() const;

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    virtual void drawTicks(QPainter*, const QRectF&);
    virtual void drawWheelBackground(QPainter*, const QRectF&);

    virtual double valueAt(const QPoint&) const;

private:
    double alignedValue(double) const;
    double boundedValue(double) const;

    QWT_DECLARE_PRIVATE(QwtWheel)
};

#endif
