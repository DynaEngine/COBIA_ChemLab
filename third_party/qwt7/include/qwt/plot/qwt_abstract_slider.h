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

#ifndef QWT_ABSTRACT_SLIDER_H
#define QWT_ABSTRACT_SLIDER_H

#include "qwt_global.h"
#include "qwt_abstract_scale.h"

/**
 * @brief An abstract base class for slider widgets with a scale
 * @details A slider widget displays a value according to a scale.
 *          The class is designed as a common super class for widgets like
 *          QwtKnob, QwtDial and QwtSlider.
 *          When the slider is not readOnly(), its value can be modified
 *          by keyboard, mouse and wheel inputs.
 *          The range of the slider is divided into a number of steps from
 *          which the value increments according to user inputs depend.
 *          Only for linear scales the number of steps correspond with
 *          a fixed step size.
 */

class QWT_EXPORT QwtAbstractSlider : public QwtAbstractScale
{
    Q_OBJECT

    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged USER true)

    Q_PROPERTY(uint totalSteps READ totalSteps WRITE setTotalSteps)
    Q_PROPERTY(uint singleSteps READ singleSteps WRITE setSingleSteps)
    Q_PROPERTY(uint pageSteps READ pageSteps WRITE setPageSteps)
    Q_PROPERTY(bool stepAlignment READ stepAlignment WRITE setStepAlignment)

    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool tracking READ isTracking WRITE setTracking)
    Q_PROPERTY(bool wrapping READ wrapping WRITE setWrapping)

    Q_PROPERTY(bool invertedControls READ invertedControls WRITE setInvertedControls)

public:
    /// Constructor for QwtAbstractSlider (English only)
    explicit QwtAbstractSlider(QWidget* parent = nullptr);

    /// Destructor for QwtAbstractSlider (English only)
    ~QwtAbstractSlider() override;

    /// Set whether the slider is valid (English only)
    void setValid(bool);

    /// Return whether the slider is valid (English only)
    bool isValid() const;

    /// Return the current value (English only)
    double value() const;

    /// Set whether wrapping is enabled (English only)
    void setWrapping(bool);

    /// Return whether wrapping is enabled (English only)
    bool wrapping() const;

    /// Set the total number of steps (English only)
    void setTotalSteps(uint);

    /// Return the total number of steps (English only)
    uint totalSteps() const;

    /// Set the number of single steps (English only)
    void setSingleSteps(uint);

    /// Return the number of single steps (English only)
    uint singleSteps() const;

    /// Set the number of page steps (English only)
    void setPageSteps(uint);

    /// Return the number of page steps (English only)
    uint pageSteps() const;

    /// Set whether step alignment is enabled (English only)
    void setStepAlignment(bool);

    /// Return whether step alignment is enabled (English only)
    bool stepAlignment() const;

    /// Set whether tracking is enabled (English only)
    void setTracking(bool);

    /// Return whether tracking is enabled (English only)
    bool isTracking() const;

    /// Set whether the slider is read-only (English only)
    void setReadOnly(bool);

    /// Return whether the slider is read-only (English only)
    bool isReadOnly() const;

    /// Set whether controls are inverted (English only)
    void setInvertedControls(bool);

    /// Return whether controls are inverted (English only)
    bool invertedControls() const;

public Q_SLOTS:
    /// Set the slider value (English only)
    void setValue(double value);

Q_SIGNALS:

    /**
     * @brief Notify a change of value
     * @details When tracking is enabled (default setting),
     *          this signal will be emitted every time the value changes.
     * @param value New value
     * @sa setTracking(), sliderMoved()
     */
    void valueChanged(double value);

    /**
     * @brief Signal emitted when the user presses the movable part of the slider
     */
    void sliderPressed();

    /**
     * @brief Signal emitted when the user releases the movable part of the slider
     */
    void sliderReleased();

    /**
     * @brief Signal emitted when the user moves the slider with the mouse
     * @param value New value
     * @sa valueChanged()
     */
    void sliderMoved(double value);

protected:
    /// Handle mouse press events (English only)
    virtual void mousePressEvent(QMouseEvent*) override;

    /// Handle mouse release events (English only)
    virtual void mouseReleaseEvent(QMouseEvent*) override;

    /// Handle mouse move events (English only)
    virtual void mouseMoveEvent(QMouseEvent*) override;

    /// Handle key press events (English only)
    virtual void keyPressEvent(QKeyEvent*) override;

    /// Handle wheel events (English only)
    virtual void wheelEvent(QWheelEvent*) override;

    // Determine what to do when the user presses a mouse button
    virtual bool isScrollPosition(const QPoint& pos) const = 0;

    // Determine the value for a new position of the movable part of the slider
    virtual double scrolledTo(const QPoint& pos) const = 0;

    /// Increment the value by a number of steps (English only)
    void incrementValue(int stepCount);

    /// Handle scale changes (English only)
    virtual void scaleChange() override;

protected:
    /// Handle slider changes (English only)
    virtual void sliderChange();

    /// Calculate incremented value (English only)
    double incrementedValue(double value, int stepCount) const;

private:
    double alignedValue(double) const;
    double boundedValue(double) const;

    QWT_DECLARE_PRIVATE(QwtAbstractSlider)
};

#endif
