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

#ifndef QWT_PLOT_HISTOGRAM_H
#define QWT_PLOT_HISTOGRAM_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"

class QwtColumnSymbol;
class QwtColumnRect;
class QColor;
class QPolygonF;

#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief QwtPlotHistogram represents a series of samples, where an interval
 *        is associated with a value ( \f$y = f([x1,x2])\f$ )
 * @details The representation depends on the style() and an optional symbol()
 *          that is displayed for each interval.
 *
 * @note The term "histogram" is used in a different way in the areas of
 *       digital image processing and statistics. Wikipedia introduces the
 *       terms "image histogram" and "color histogram" to avoid confusions.
 *       While "image histograms" can be displayed by a QwtPlotCurve there
 *       is no applicable plot item for a "color histogram" yet.
 *
 * @sa QwtPlotBarChart, QwtPlotMultiBarChart
 *
 */

class QWT_EXPORT QwtPlotHistogram : public QwtPlotSeriesItem, public QwtSeriesStore< QwtIntervalSample >
{
public:
    /**
     * @brief Histogram styles
     * @details The default style is QwtPlotHistogram::Columns.
     * @sa setStyle(), style(), setSymbol(), symbol(), setBaseline()
     *
     */
    enum HistogramStyle
    {
        /**
         * Draw an outline around the area, that is build by all intervals
         * using the pen() and fill it with the brush(). The outline style
         * requires, that the intervals are in increasing order and
         * not overlapping.
         *
         */
        Outline,

        /**
         * Draw a column for each interval. When a symbol() has been set
         * the symbol is used otherwise the column is displayed as
         * plain rectangle using pen() and brush().
         *
         */
        Columns,

        /**
         * Draw a simple line using the pen() for each interval.
         *
         */
        Lines,

        /**
         * Styles >= UserStyle are reserved for derived
         * classes that overload drawSeries() with
         * additional application specific ways to display a histogram.
         *
         */
        UserStyle = 100
    };

    // Constructor
    explicit QwtPlotHistogram(const QString& title = QString());

    // Constructor with QwtText title
    explicit QwtPlotHistogram(const QwtText& title);

    // Destructor
    ~QwtPlotHistogram() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Attach the histogram to a plot (applies color cycle if pen/brush not user-set)
    void attach(QwtPlot* plot) override;

    // Set pen
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);

    // Set pen
    void setPen(const QPen&);

    // Get pen
    const QPen& pen() const;

    // Set brush
    void setBrush(const QBrush&);

    // Get brush
    const QBrush& brush() const;

    // Set samples from a vector
    void setSamples(const QVector< QwtIntervalSample >&);

    // Set samples from a series data
    void setSamples(QwtSeriesData< QwtIntervalSample >*);

    // Set baseline
    void setBaseline(double);

    // Get baseline
    double baseline() const;

    // Set histogram style
    void setStyle(HistogramStyle style);

    // Get histogram style
    HistogramStyle style() const;

    // Set symbol
    void setSymbol(const QwtColumnSymbol*);

    // Get symbol
    const QwtColumnSymbol* symbol() const;

    // Draw the series
    virtual void drawSeries(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            int from,
                            int to) const override;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    // Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

protected:
    /**
     * @brief Get the column rectangle
     */
    virtual QwtColumnRect columnRect(const QwtIntervalSample&, const QwtScaleMap&, const QwtScaleMap&) const;

    /**
     * @brief Draw a column
     */
    virtual void drawColumn(QPainter*, const QwtColumnRect&, const QwtIntervalSample&) const;

    /**
     * @brief Draw columns
     */
    void drawColumns(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, int from, int to) const;

    /**
     * @brief Draw outline
     */
    void drawOutline(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, int from, int to) const;

    /**
     * @brief Draw lines
     */
    void drawLines(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, int from, int to) const;

private:
    /**
     * @brief Initialize the histogram
     */
    void init();

    /**
     * @brief Flush polygon
     */
    void flushPolygon(QPainter*, double baseLine, QPolygonF&) const;

    QWT_DECLARE_PRIVATE(QwtPlotHistogram)
};

#endif
