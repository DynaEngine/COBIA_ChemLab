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

#ifndef QWT_DATE_SCALE_DRAW_H
#define QWT_DATE_SCALE_DRAW_H

#include "qwt_global.h"
#include "qwt_scale_draw.h"
#include "qwt_date.h"

/**
 * @brief A class for drawing datetime scales
 * @details QwtDateScaleDraw displays values as datetime labels.
 *          The format of the labels depends on the alignment of
 *          the major tick labels.
 *
 *          The default format strings are:
 *          - Millisecond: "hh:mm:ss:zzz\nddd dd MMM yyyy"
 *          - Second: "hh:mm:ss\nddd dd MMM yyyy"
 *          - Minute: "hh:mm\nddd dd MMM yyyy"
 *          - Hour: "hh:mm\nddd dd MMM yyyy"
 *          - Day: "ddd dd MMM yyyy"
 *          - Week: "Www yyyy"
 *          - Month: "MMM yyyy"
 *          - Year: "yyyy"
 *
 *          The format strings can be modified using setDateFormat()
 *          or individually for each tick label by overloading dateFormatOfDate().
 *
 *          Usually QwtDateScaleDraw is used in combination with
 *          QwtDateScaleEngine, that calculates scales for datetime intervals.
 * @sa QwtDateScaleEngine, QwtPlot::setAxisScaleDraw()
 *
 */
class QWT_EXPORT QwtDateScaleDraw : public QwtScaleDraw
{
public:
    /// Constructor with time specification
    explicit QwtDateScaleDraw(Qt::TimeSpec timeSpec = Qt::LocalTime);

    /// Destructor
    ~QwtDateScaleDraw() override;

    /// Set the default format string for a datetime interval type
    void setDateFormat(QwtDate::IntervalType, const QString&);

    /// Get the default format string for a datetime interval type
    QString dateFormat(QwtDate::IntervalType) const;

    /// Set the time specification used for the tick labels
    void setTimeSpec(Qt::TimeSpec);

    /// Get the time specification used for the tick labels
    Qt::TimeSpec timeSpec() const;

    /// Set the offset in seconds from Coordinated Universal Time
    void setUtcOffset(int seconds);

    /// Get the offset in seconds from Coordinated Universal Time
    int utcOffset() const;

    /// Set how to identify the first week of a year
    void setWeek0Type(QwtDate::Week0Type);

    /// Get how to identify the first week of a year
    QwtDate::Week0Type week0Type() const;

    /// Convert a value into its representing label
    virtual QwtText label(double) const override;

    /// Translate a double value into a QDateTime object
    QDateTime toDateTime(double) const;

protected:
    virtual QwtDate::IntervalType intervalType(const QwtScaleDiv&) const;

    virtual QString dateFormatOfDate(const QDateTime&, QwtDate::IntervalType) const;

private:
    QWT_DECLARE_PRIVATE(QwtDateScaleDraw)
};

#endif
