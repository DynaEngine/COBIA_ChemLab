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

#ifndef QWT_DATE_H
#define QWT_DATE_H

#include "qwtcore_global.h"
#include <qdatetime.h>

/**
 * @brief A collection of methods around date/time values
 *
 * Qt offers convenient classes for dealing with date/time values,
 * but Qwt uses coordinate systems that are based on doubles.
 * QwtDate offers methods to translate from QDateTime to double and v.v.
 *
 * A double is interpreted as the number of milliseconds since
 * 1970-01-01T00:00:00 Universal Coordinated Time - also known
 * as "The Epoch".
 *
 * While the range of the Julian day in Qt4 is limited to [0, MAX_INT],
 * Qt5 stores it as qint64 offering a huge range of valid dates.
 * As the significance of a double is below this ( assuming a
 * fraction of 52 bits ) the translation is not
 * bijective with rounding errors for dates very far from Epoch.
 * For a resolution of 1 ms those start to happen for dates above the
 * year 144683.
 *
 * An axis for a date/time interval is expected to be aligned
 * and divided in time/date units like seconds, minutes, ...
 * QwtDate offers several algorithms that are needed to
 * calculate these axes.
 *
 * @sa QwtDateScaleEngine, QwtDateScaleDraw, QDate, QTime
 *
 */
class QWTCORE_EXPORT QwtDate
{
public:
    /**
     * @brief How to identify the first week of year differs between countries
     */
    enum Week0Type
    {
        /**
         * According to ISO 8601 the first week of a year is defined
         * as "the week with the year's first Thursday in it".
         * FirstThursday corresponds to the numbering that is
         * implemented in QDate::weekNumber().
         */
        FirstThursday,

        /**
         * "The week with January 1 in it."
         * In the U.S. this definition is more common than FirstThursday.
         */
        FirstDay
    };

    /**
     * @brief Classification of a time interval
     *
     * Time intervals need to be classified to decide how to
     * align and divide it.
     */
    enum IntervalType
    {
        //! Millisecond
        Millisecond,

        //! Second
        Second,

        //! Minute
        Minute,

        //! Hour
        Hour,

        //! Day
        Day,

        //! Week
        Week,

        //! Month
        Month,

        //! Year
        Year
    };

    enum
    {
        //! The Julian day of "The Epoch"
        JulianDayForEpoch = 2440588
    };

    // Get minimum date
    static QDate minDate();
    // Get maximum date
    static QDate maxDate();

    // Convert double to QDateTime
    static QDateTime toDateTime(double value, Qt::TimeSpec = Qt::UTC);

    // Convert QDateTime to double
    static double toDouble(const QDateTime&);

    // Ceil datetime to interval type
    static QDateTime ceil(const QDateTime&, IntervalType);
    // Floor datetime to interval type
    static QDateTime floor(const QDateTime&, IntervalType);

    // Get date of week 0
    static QDate dateOfWeek0(int year, Week0Type);
    // Get week number
    static int weekNumber(const QDate&, Week0Type);

    // Get UTC offset in seconds
    static int utcOffset(const QDateTime&);

    // Format datetime to string
    static QString toString(const QDateTime&, const QString& format, Week0Type);
};

#endif
