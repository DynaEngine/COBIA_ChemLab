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

#ifndef QWT_ABSTRACT_SCALE_H
#define QWT_ABSTRACT_SCALE_H

#include "qwt_global.h"
#include <qwidget.h>

class QwtScaleEngine;
class QwtAbstractScaleDraw;
class QwtScaleDiv;
class QwtScaleMap;
class QwtInterval;

/**
 * @brief An abstract base class for widgets having a scale
 * @details The scale of an QwtAbstractScale is determined by a QwtScaleDiv
 *          definition that contains the boundaries and the ticks of the scale.
 *          The scale is painted using a QwtScaleDraw object.
 *          The scale division might be assigned explicitly - but usually
 *          it is calculated from the boundaries using a QwtScaleEngine.
 *          The scale engine also decides the type of transformation of the scale
 *          (linear, logarithmic ...).
 */

class QWT_EXPORT QwtAbstractScale : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(double lowerBound READ lowerBound WRITE setLowerBound)
    Q_PROPERTY(double upperBound READ upperBound WRITE setUpperBound)

    Q_PROPERTY(int scaleMaxMajor READ scaleMaxMajor WRITE setScaleMaxMajor)
    Q_PROPERTY(int scaleMaxMinor READ scaleMaxMinor WRITE setScaleMaxMinor)

    Q_PROPERTY(double scaleStepSize READ scaleStepSize WRITE setScaleStepSize)

public:
    // Constructor for QwtAbstractScale
    explicit QwtAbstractScale(QWidget* parent = nullptr);

    // Destructor for QwtAbstractScale
    ~QwtAbstractScale() override;

    // Set scale by lower and upper bounds
    void setScale(double lowerBound, double upperBound);

    // Set scale by interval
    void setScale(const QwtInterval&);

    // Set scale by scale division
    void setScale(const QwtScaleDiv&);

    // Return the current scale division
    const QwtScaleDiv& scaleDiv() const;

    // Set the lower bound of the scale
    void setLowerBound(double value);

    // Return the lower bound of the scale
    double lowerBound() const;

    // Set the upper bound of the scale
    void setUpperBound(double value);

    // Return the upper bound of the scale
    double upperBound() const;

    // Set the step size for scale calculation
    void setScaleStepSize(double stepSize);

    // Return the current step size hint
    double scaleStepSize() const;

    // Set the maximum number of major tick intervals
    void setScaleMaxMajor(int ticks);

    // Return the maximum number of minor tick intervals
    int scaleMaxMinor() const;

    // Set the maximum number of minor tick intervals
    void setScaleMaxMinor(int ticks);

    // Return the maximum number of major tick intervals
    int scaleMaxMajor() const;

    // Set the scale engine
    void setScaleEngine(QwtScaleEngine*);

    // Return the scale engine (const version)
    const QwtScaleEngine* scaleEngine() const;

    // Return the scale engine (non-const version)
    QwtScaleEngine* scaleEngine();

    // Transform a scale value to widget coordinates
    int transform(double) const;

    // Transform a widget coordinate to scale value
    double invTransform(int) const;

    // Return true if scale is inverted
    bool isInverted() const;

    // Return the minimum boundary value
    double minimum() const;

    // Return the maximum boundary value
    double maximum() const;

    // Return the scale map
    const QwtScaleMap& scaleMap() const;

protected:
    /// Handle change events (English only)
    virtual void changeEvent(QEvent*) override;

    /// Recalculate scale and update scale draw (English only)
    void rescale(double lowerBound, double upperBound, double stepSize);

    /// Set the scale draw object (English only)
    void setAbstractScaleDraw(QwtAbstractScaleDraw*);

    /// Return the scale draw (const version) (English only)
    const QwtAbstractScaleDraw* abstractScaleDraw() const;

    /// Return the scale draw (non-const version) (English only)
    QwtAbstractScaleDraw* abstractScaleDraw();

    /// Update the scale draw (English only)
    void updateScaleDraw();

    /// Notify about scale changes (English only)
    virtual void scaleChange();

private:
    QWT_DECLARE_PRIVATE(QwtAbstractScale)
};

#endif
