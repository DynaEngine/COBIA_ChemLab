/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_DATA_ACCESS_H
#define QWT_PLOT_DATA_ACCESS_H

#include "qwt_global.h"
#include "qwt_series_store.h"

#include <qpoint.h>
#include <qrect.h>
#include <qvector.h>

class QPainterPath;
class QwtPlotItem;
class QwtIntervalSample;
class QwtPoint3D;
class QwtOHLCSample;
class QwtSetSample;
class QwtVectorFieldSample;
class QwtBoxSample;

/**
 * @brief Utility class for reading and writing sample data from QwtPlotItem instances
 * @details QwtPlotDataAccess provides type-safe static methods to extract or set
 *          sample data from any QwtPlotItem. The methods use the item's rtti to
 *          dispatch to the correct underlying QwtSeriesStore type.
 *
 *          Supported data types:
 *          - QPointF: QwtPlotCurve, QwtPlotBarChart
 *          - QwtIntervalSample: QwtPlotIntervalCurve, QwtPlotHistogram
 *          - QwtPoint3D: QwtPlotSpectroCurve
 *          - QwtOHLCSample: QwtPlotTradingCurve
 *          - QwtSetSample: QwtPlotMultiBarChart
 *          - QwtVectorFieldSample: QwtPlotVectorField
 *          - QwtBoxSample: QwtPlotBoxChart
 *
 * @code
 * QwtPlotItem* item = ...;
 * QVector<QPointF> data = Qwt::QwtPlotDataAccess::xySamples(item);
 * if (!data.isEmpty()) {
 *     qDebug() << "First point:" << data.first();
 * }
 * @endcode
 *
 * @sa QwtPlotItemInfo, QwtSeriesStore
 */
class QWT_EXPORT QwtPlotDataAccess
{
public:
    // ---- QPointF data (QwtPlotCurve, QwtPlotBarChart) ----

    // Extract XY samples as QVector<QPointF>
    static QVector< QPointF > xySamples(const QwtPlotItem* item);

    // Extract XY samples into separate x and y vectors
    static void xySamples(const QwtPlotItem* item, QVector< double >& x, QVector< double >& y);

    // Set XY samples from QVector<QPointF>
    static bool setXYSamples(QwtPlotItem* item, const QVector< QPointF >& data);

    // Set XY samples from separate x and y vectors
    static bool setXYSamples(QwtPlotItem* item, const QVector< double >& x, const QVector< double >& y);

    // ---- QwtIntervalSample data (QwtPlotIntervalCurve, QwtPlotHistogram) ----

    // Extract interval samples
    static QVector< QwtIntervalSample > intervalSamples(const QwtPlotItem* item);

    // Set interval samples
    static bool setIntervalSamples(QwtPlotItem* item, const QVector< QwtIntervalSample >& data);

    // ---- QwtPoint3D data (QwtPlotSpectroCurve) ----

    // Extract 3D point samples
    static QVector< QwtPoint3D > xyzSamples(const QwtPlotItem* item);

    // Set 3D point samples
    static bool setXyzSamples(QwtPlotItem* item, const QVector< QwtPoint3D >& data);

    // ---- QwtOHLCSample data (QwtPlotTradingCurve) ----

    // Extract OHLC samples
    static QVector< QwtOHLCSample > ohlcSamples(const QwtPlotItem* item);

    // Set OHLC samples
    static bool setOhlcSamples(QwtPlotItem* item, const QVector< QwtOHLCSample >& data);

    // ---- QwtSetSample data (QwtPlotMultiBarChart) ----

    // Extract set samples
    static QVector< QwtSetSample > setSamples(const QwtPlotItem* item);

    // Set set samples
    static bool setSetSamples(QwtPlotItem* item, const QVector< QwtSetSample >& data);

    // ---- QwtVectorFieldSample data (QwtPlotVectorField) ----

    // Extract vector field samples
    static QVector< QwtVectorFieldSample > vectorFieldSamples(const QwtPlotItem* item);

    // Set vector field samples
    static bool setVectorFieldSamples(QwtPlotItem* item, const QVector< QwtVectorFieldSample >& data);

    // ---- QwtBoxSample data (QwtPlotBoxChart) ----

    // Extract box samples
    static QVector< QwtBoxSample > boxSamples(const QwtPlotItem* item);

    // Set box samples
    static bool setBoxSamples(QwtPlotItem* item, const QVector< QwtBoxSample >& data);

    // ---- Generic template: extract from QwtSeriesStore<T> ----

    // Extract all samples from a QwtSeriesStore
    template< typename T >
    static QVector< T > samples(const QwtSeriesStore< T >* store);

    // Extract a range of samples from a QwtSeriesStore
    template< typename T >
    static QVector< T > samples(const QwtSeriesStore< T >* store, int from, int to);

    // ---- Range-based extraction ----

    // Extract XY samples within a rectangular range
    static QVector< QPointF > xySamplesInRange(const QwtPlotItem* item, const QRectF& range);

    // Extract XY samples within a path range
    static QVector< QPointF > xySamplesInRange(const QwtPlotItem* item, const QPainterPath& range);

    // ---- Range-based removal ----

    // Remove XY samples within a rectangular range, return count removed
    static int removeSamplesInRange(QwtPlotItem* item, const QRectF& range);

    // Remove XY samples within a path range, return count removed
    static int removeSamplesInRange(QwtPlotItem* item, const QPainterPath& range);
};

// Template implementations

template< typename T >
QVector< T > QwtPlotDataAccess::samples(const QwtSeriesStore< T >* store)
{
    QVector< T > result;
    if (!store)
        return result;

    const size_t size = store->dataSize();
    result.reserve(static_cast< int >(size));
    for (size_t i = 0; i < size; ++i)
        result.append(store->sample(i));

    return result;
}

template< typename T >
QVector< T > QwtPlotDataAccess::samples(const QwtSeriesStore< T >* store, int from, int to)
{
    QVector< T > result;
    if (!store || from < 0)
        return result;

    const int size = static_cast< int >(store->dataSize());
    if (to < 0 || to >= size)
        to = size - 1;

    for (int i = from; i <= to; ++i)
        result.append(store->sample(static_cast< size_t >(i)));

    return result;
}

#endif  // QWT_PLOT_DATA_ACCESS_H
