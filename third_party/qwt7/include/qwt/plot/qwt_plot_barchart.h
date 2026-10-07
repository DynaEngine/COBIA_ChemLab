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

#ifndef QWT_PLOT_BAR_CHART_H
#define QWT_PLOT_BAR_CHART_H

#include "qwt_global.h"
#include "qwt_plot_abstract_barchart.h"
#include "qwt_column_symbol.h"

class QwtColumnRect;
template< typename T >
class QwtSeriesData;

/**
 * @brief QwtPlotBarChart displays a series of values as bars
 * @details Each bar might be customized individually by implementing
 *          a specialSymbol(). Otherwise it is rendered using a default symbol.
 *
 *          Depending on its orientation() the bars are displayed horizontally
 *          or vertically. The bars cover the interval between the baseline()
 *          and the value.
 *
 *          By activating the LegendBarTitles mode each sample will have
 *          its own entry on the legend.
 *
 *          The most common use case of a bar chart is to display a
 *          list of y coordinates, where the x coordinate is simply the index
 *          in the list. But for other situations ( f.e. when values are related
 *          to dates ) it is also possible to set x coordinates explicitly.
 *
 * @sa QwtPlotMultiBarChart, QwtPlotHistogram, QwtPlotCurve::Sticks,
 *     QwtPlotSeriesItem::orientation(), QwtPlotAbstractBarChart::baseline()
 */
class QWT_EXPORT QwtPlotBarChart : public QwtPlotAbstractBarChart, public QwtSeriesStore< QPointF >
{
public:
    /**
     * @brief Legend modes
     * @details The default setting is QwtPlotBarChart::LegendChartTitle.
     * @sa setLegendMode(), legendMode()
     */
    enum LegendMode
    {
        /**
         * One entry on the legend showing the default symbol
         * and the title() of the chart
         * @sa QwtPlotItem::title()
         */
        LegendChartTitle,

        /**
         * One entry for each value showing the individual symbol
         * of the corresponding bar and the bar title.
         * @sa specialSymbol(), barTitle()
         */
        LegendBarTitles
    };

    // Constructor
    explicit QwtPlotBarChart(const QString& title = QString());
    // Constructor with QwtText title
    explicit QwtPlotBarChart(const QwtText& title);

    // Destructor
    ~QwtPlotBarChart() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Attach the bar chart to a plot (applies color cycle if pen/brush not user-set)
    void attach(QwtPlot* plot) override;

    // Set samples from QVector<QPointF>
    void setSamples(const QVector< QPointF >&);
    // Set samples from QVector<double>
    void setSamples(const QVector< double >&);
    // Set samples from QwtSeriesData
    void setSamples(QwtSeriesData< QPointF >*);

    // Set symbol
    void setSymbol(QwtColumnSymbol*);
    // Get symbol
    const QwtColumnSymbol* symbol() const;

    // Set pen
    void setPen(const QPen& p);
    // Get pen
    QPen pen() const;

    // Set brush
    void setBrush(const QBrush& b);
    // Get brush
    QBrush brush() const;

    // Set frame style
    void setFrameStyle(QwtColumnSymbol::FrameStyle f);
    // Get frame style
    QwtColumnSymbol::FrameStyle frameStyle() const;

    // Set legend mode
    void setLegendMode(LegendMode);
    // Get legend mode
    LegendMode legendMode() const;

    // Draw the series
    virtual void drawSeries(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            int from,
                            int to) const override;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    // Get special symbol for a sample
    virtual QwtColumnSymbol* specialSymbol(int sampleIndex, const QPointF&) const;

    // Get bar title for a sample
    virtual QwtText barTitle(int sampleIndex) const;

protected:
    /// Draw a sample
    virtual void drawSample(QPainter* painter,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            const QwtInterval& boundingInterval,
                            int index,
                            const QPointF& sample) const;

    /// Draw a bar
    virtual void drawBar(QPainter*, int sampleIndex, const QPointF& sample, const QwtColumnRect&) const;

    /// Get column rectangle
    QwtColumnRect columnRect(const QwtScaleMap& xMap,
                             const QwtScaleMap& yMap,
                             const QRectF& canvasRect,
                             const QwtInterval& boundingInterval,
                             const QPointF& sample) const;

    /// Get legend data
    QList< QwtLegendData > legendData() const override;
    /// Get legend icon
    QwtGraphic legendIcon(int index, const QSizeF&) const override;

private:
    /// Initialize the bar chart
    void init();

    QWT_DECLARE_PRIVATE(QwtPlotBarChart)
};

#endif
