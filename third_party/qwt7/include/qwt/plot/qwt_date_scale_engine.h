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

#ifndef QWT_DATE_SCALE_ENGINE_H
#define QWT_DATE_SCALE_ENGINE_H

#include "qwt_global.h"
#include "qwt_date.h"
#include "qwt_scale_engine.h"

/**
 * @brief A scale engine for date/time values
 * @details QwtDateScaleEngine builds scales from time intervals.
 *          Together with QwtDateScaleDraw it can be used for
 *          axes according to date/time values.
 *
 *          Years, months, weeks, days, hours and minutes are organized
 *          in steps with non constant intervals. QwtDateScaleEngine
 *          classifies intervals and aligns the boundaries and tick positions
 *          according to this classification.
 *
 *          QwtDateScaleEngine supports representations depending
 *          on Qt::TimeSpec specifications. The valid range for scales
 *          is limited by the range of QDateTime, that differs
 *          between Qt4 and Qt5.
 *
 *          Datetime values are expected as the number of milliseconds since
 *          1970-01-01T00:00:00 Universal Coordinated Time - also known
 *          as "The Epoch", that can be converted to QDateTime using
 *          QwtDate::toDateTime().
 * @sa QwtDate, QwtPlot::setAxisScaleEngine(), QwtAbstractScale::setScaleEngine()
 *
 */
class QWT_EXPORT QwtDateScaleEngine : public QwtLinearScaleEngine
{
public:
    /// Constructor with time specification
    explicit QwtDateScaleEngine(Qt::TimeSpec = Qt::LocalTime);

    /// Destructor
    ~QwtDateScaleEngine() override;

    /// Set the time specification used by the engine
    void setTimeSpec(Qt::TimeSpec);

    /// Get the time specification used by the engine
    Qt::TimeSpec timeSpec() const;

    /// Set the offset in seconds from Coordinated Universal Time
    void setUtcOffset(int seconds);

    /// Get the offset in seconds from Coordinated Universal Time
    int utcOffset() const;

    /// Set how to identify the first week of a year
    void setWeek0Type(QwtDate::Week0Type);

    /// Get how to identify the first week of a year
    QwtDate::Week0Type week0Type() const;

    /// Set upper limit for the number of weeks
    void setMaxWeeks(int);

    /// Get upper limit for the number of weeks
    int maxWeeks() const;

    /// Align and divide an interval
    virtual void autoScale(int maxNumSteps, double& x1, double& x2, double& stepSize) const override;

    /// Calculate a scale division for a date/time interval
    virtual QwtScaleDiv
    divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize = 0.0) const override;

    /// Classification of a date/time interval division
    virtual QwtDate::IntervalType intervalType(const QDateTime&, const QDateTime&, int maxSteps) const;

    /// Translate a double value into a QDateTime object
    QDateTime toDateTime(double) const;

protected:
    virtual QDateTime alignDate(const QDateTime&, double stepSize, QwtDate::IntervalType, bool up) const;

private:
    QwtScaleDiv
    buildScaleDiv(const QDateTime&, const QDateTime&, int maxMajorSteps, int maxMinorSteps, QwtDate::IntervalType) const;

private:
    QWT_DECLARE_PRIVATE(QwtDateScaleEngine)
};

#endif
