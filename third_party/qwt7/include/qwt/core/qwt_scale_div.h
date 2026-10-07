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

#ifndef QWT_SCALE_DIV_H
#define QWT_SCALE_DIV_H

#include "qwtcore_global.h"
#include <qlist.h>

class QwtInterval;

/**
 * @brief A class representing a scale division
 *
 * A Qwt scale is defined by its boundaries and 3 list for the positions of the major, medium and minor ticks.
 *
 * The upperBound() might be smaller than the lowerBound() to indicate inverted scales.
 *
 * Scale divisions can be calculated from a QwtScaleEngine.
 *
 * @sa QwtScaleEngine::divideScale(), QwtPlot::setAxisScaleDiv(), QwtAbstractSlider::setScaleDiv()
 */
class QWTCORE_EXPORT QwtScaleDiv
{
public:
    //! Scale tick types
    enum TickType
    {
        //! No ticks
        NoTick = -1,

        //! Minor ticks
        MinorTick,

        //! Medium ticks
        MediumTick,

        //! Major ticks
        MajorTick,

        //! Number of valid tick types
        NTickTypes
    };

    // Constructor with lower and upper bounds
    explicit QwtScaleDiv(double lowerBound = 0.0, double upperBound = 0.0);

    // Constructor with interval and ticks array
    explicit QwtScaleDiv(const QwtInterval&, QList< double >[ NTickTypes ]);

    // Constructor with bounds and ticks array
    explicit QwtScaleDiv(double lowerBound, double upperBound, QList< double >[ NTickTypes ]);

    // Constructor with bounds and separate tick lists
    explicit QwtScaleDiv(double lowerBound,
                         double upperBound,
                         const QList< double >& minorTicks,
                         const QList< double >& mediumTicks,
                         const QList< double >& majorTicks);

    // Copy constructor
    QwtScaleDiv(const QwtScaleDiv&);
    // Move constructor
    QwtScaleDiv(QwtScaleDiv&&) noexcept = default;
    // Copy assignment
    QwtScaleDiv& operator=(const QwtScaleDiv&);
    // Move assignment
    QwtScaleDiv& operator=(QwtScaleDiv&&) noexcept = default;

    // Equality operator
    bool operator==(const QwtScaleDiv&) const;
    // Inequality operator
    bool operator!=(const QwtScaleDiv&) const;
    // Fuzzy comparison
    bool fuzzyCompare(const QwtScaleDiv& other) const;

    // Set the interval
    void setInterval(double lowerBound, double upperBound);
    // Set the interval from QwtInterval
    void setInterval(const QwtInterval&);
    // Get the interval
    QwtInterval interval() const;

    // Set the lower bound
    void setLowerBound(double) noexcept;
    // Get the lower bound
    double lowerBound() const noexcept;

    // Set the upper bound
    void setUpperBound(double) noexcept;
    // Get the upper bound
    double upperBound() const noexcept;

    // Get the range (upper - lower)
    double range() const noexcept;

    // Check if value is within bounds
    bool contains(double value) const;

    // Set ticks for a specific tick type
    void setTicks(int tickType, const QList< double >&);
    // Get ticks for a specific tick type
    QList< double > ticks(int tickType) const;

    // Check if scale division is empty
    bool isEmpty() const;
    // Check if scale is increasing
    bool isIncreasing() const;

    // Invert the scale division
    void invert();
    // Get inverted scale division
    QwtScaleDiv inverted() const;

    // Get bounded scale division
    QwtScaleDiv bounded(double lowerBound, double upperBound) const;

private:
    double m_lowerBound { 0.0 };
    double m_upperBound { 0.0 };
    QList< double > m_ticks[ NTickTypes ];
};

Q_DECLARE_TYPEINFO(QwtScaleDiv, Q_MOVABLE_TYPE);

#ifndef QT_NO_DEBUG_STREAM
QWTCORE_EXPORT QDebug operator<<(QDebug, const QwtScaleDiv&);
#endif

#endif
