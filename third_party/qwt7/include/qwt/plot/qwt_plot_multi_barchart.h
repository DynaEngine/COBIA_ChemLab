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

#ifndef QWT_PLOT_MULTI_BAR_CHART_H
#define QWT_PLOT_MULTI_BAR_CHART_H

#include "qwt_global.h"
#include "qwt_plot_abstract_barchart.h"

class QwtColumnRect;
class QwtColumnSymbol;
template< typename T >
class QwtSeriesData;

/**
 * @brief QwtPlotMultiBarChart displays a series of samples that consist each of a set of values
 * @details Each value is displayed as a bar, the bars of each set can be organized
 *          side by side or accumulated.
 *
 *          Each bar of a set is rendered by a QwtColumnSymbol, that is set by setSymbol().
 *          The bars of different sets use the same symbols. Exceptions are possible
 *          by overloading specialSymbol() or overloading drawBar().
 *
 *          Depending on its orientation() the bars are displayed horizontally
 *          or vertically. The bars cover the interval between the baseline()
 *          and the value.
 *
 *          In opposite to most other plot items, QwtPlotMultiBarChart returns more
 *          than one entry for the legend - one for each symbol.
 *
 * @sa QwtPlotBarChart, QwtPlotHistogram
 * @sa QwtPlotSeriesItem::orientation(), QwtPlotAbstractBarChart::baseline()
 *
 */
class QWT_EXPORT QwtPlotMultiBarChart : public QwtPlotAbstractBarChart, public QwtSeriesStore< QwtSetSample >
{
public:
    /**
     * @brief Chart styles
     * @details The default setting is QwtPlotMultiBarChart::Grouped.
     * @sa setStyle(), style()
     *
     */
    enum ChartStyle
    {
        /// The bars of a set are displayed side by side
        Grouped,

        /**
         * The bars are displayed on top of each other accumulating
         * to a single bar. All values of a set need to have the same
         * sign.
         *
         */
        Stacked
    };

    /// Constructor
    explicit QwtPlotMultiBarChart(const QString& title = QString());
    /// Constructor with title
    explicit QwtPlotMultiBarChart(const QwtText& title);

    /// Destructor
    ~QwtPlotMultiBarChart() override;

    /// Get the runtime type information
    virtual int rtti() const override;

    /// Attach to a plot (applies color cycle to symbols if none user-set)
    void attach(QwtPlot* plot) override;

    /// Set the bar titles
    void setBarTitles(const QList< QwtText >&);
    /// Get the bar titles
    QList< QwtText > barTitles() const;

    /// Set samples from a vector of QwtSetSample
    void setSamples(const QVector< QwtSetSample >&);
    /// Set samples from a vector of vectors
    void setSamples(const QVector< QVector< double > >&);
    /// Set samples from a series data
    void setSamples(QwtSeriesData< QwtSetSample >*);

    /// Set the chart style
    void setStyle(ChartStyle style);
    /// Get the chart style
    ChartStyle style() const;

    /// Set the symbol for a specific value index
    void setSymbol(int valueIndex, QwtColumnSymbol*);
    /// Get the symbol for a specific value index
    const QwtColumnSymbol* symbol(int valueIndex) const;

    /// Reset the symbol map
    void resetSymbolMap();

    /// Draw the series
    virtual void drawSeries(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            int from,
                            int to) const override;

    /// Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    /// Get the legend data
    virtual QList< QwtLegendData > legendData() const override;

    /// Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

protected:
    /// Get the symbol for a specific value index
    QwtColumnSymbol* symbol(int valueIndex);

    /// Get a special symbol for a specific sample and value index
    virtual QwtColumnSymbol* specialSymbol(int sampleIndex, int valueIndex) const;

    /// Draw a sample
    virtual void drawSample(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            const QwtInterval& boundingInterval,
                            int index,
                            const QwtSetSample&) const;

    /// Draw a bar
    virtual void drawBar(QPainter*, int sampleIndex, int valueIndex, const QwtColumnRect&) const;

    /// Draw stacked bars
    void drawStackedBars(QPainter*,
                         const QwtScaleMap& xMap,
                         const QwtScaleMap& yMap,
                         const QRectF& canvasRect,
                         int index,
                         double sampleWidth,
                         const QwtSetSample&) const;

    /// Draw grouped bars
    void drawGroupedBars(QPainter*,
                         const QwtScaleMap& xMap,
                         const QwtScaleMap& yMap,
                         const QRectF& canvasRect,
                         int index,
                         double sampleWidth,
                         const QwtSetSample&) const;

private:
    /// Initialize the multi-bar chart
    void init();

    QWT_DECLARE_PRIVATE(QwtPlotMultiBarChart)
};

#endif
