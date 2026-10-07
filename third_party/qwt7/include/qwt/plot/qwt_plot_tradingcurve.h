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

#ifndef QWT_PLOT_TRADING_CURVE_H
#define QWT_PLOT_TRADING_CURVE_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"

/**
 * @brief QwtPlotTradingCurve illustrates movements in the price of a financial instrument over time
 * @details QwtPlotTradingCurve supports candlestick or bar ( OHLC ) charts
 *          that are used in the domain of technical analysis.
 *
 *          While the length ( height or width depending on orientation() )
 *          of each symbol depends on the corresponding OHLC sample the size
 *          of the other dimension can be controlled using:
 *
 *          - setSymbolExtent()
 *          - setSymbolMinWidth()
 *          - setSymbolMaxWidth()
 *
 *          The extent is a size in scale coordinates, so that the symbol width
 *          is increasing when the plot is zoomed in. Minimum/Maximum width
 *          is in widget coordinates independent from the zoom level.
 *          When setting the minimum and maximum to the same value, the width of
 *          the symbol is fixed.
 *
 */
class QWT_EXPORT QwtPlotTradingCurve : public QwtPlotSeriesItem, public QwtSeriesStore< QwtOHLCSample >
{
public:
    /**
     * @brief Symbol styles
     * @details The default setting is QwtPlotSeriesItem::CandleStick.
     * @sa setSymbolStyle(), symbolStyle()
     *
     */
    enum SymbolStyle
    {
        /// Nothing is displayed
        NoSymbol = -1,

        /**
         * A line on the chart shows the price range (the highest and lowest
         * prices) over one unit of time, e.g. one day or one hour.
         * Tick marks project from each side of the line indicating the
         * opening and closing price.
         *
         */
        Bar,

        /**
         * The range between opening/closing price are displayed as
         * a filled box. The fill brush depends on the direction of the
         * price movement. The box is connected to the highest/lowest
         * values by lines.
         *
         */
        CandleStick,

        /**
         * SymbolTypes >= UserSymbol are displayed by drawUserSymbol(),
         * that needs to be overloaded and implemented in derived
         * curve classes.
         *
         * @sa drawUserSymbol()
         *
         */
        UserSymbol = 100
    };

    /**
     * @brief Direction of a price movement
     *
     */
    enum Direction
    {
        /// The closing price is higher than the opening price
        Increasing,

        /// The closing price is lower than the opening price
        Decreasing
    };

    /**
     * @brief Paint attributes
     * @details Attributes to modify the drawing algorithm.
     * @sa setPaintAttribute(), testPaintAttribute()
     *
     */
    enum PaintAttribute
    {
        /// Check if a symbol is on the plot canvas before painting it.
        ClipSymbols = 0x01
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    // Constructor
    explicit QwtPlotTradingCurve(const QString& title = QString());
    // Constructor with title
    explicit QwtPlotTradingCurve(const QwtText& title);

    // Destructor
    ~QwtPlotTradingCurve() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set the samples
    void setSamples(const QVector< QwtOHLCSample >&);
    // Set the samples
    void setSamples(QwtSeriesData< QwtOHLCSample >*);

    // Set the symbol style
    void setSymbolStyle(SymbolStyle style);
    // Get the symbol style
    SymbolStyle symbolStyle() const;

    // Set the symbol pen
    void setSymbolPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    // Set the symbol pen
    void setSymbolPen(const QPen&);
    // Get the symbol pen
    QPen symbolPen() const;

    // Set the symbol brush
    void setSymbolBrush(Direction, const QBrush&);
    // Get the symbol brush
    QBrush symbolBrush(Direction) const;

    // Set the symbol extent
    void setSymbolExtent(double);
    // Get the symbol extent
    double symbolExtent() const;

    // Set the minimum symbol width
    void setMinSymbolWidth(double);
    // Get the minimum symbol width
    double minSymbolWidth() const;

    // Set the maximum symbol width
    void setMaxSymbolWidth(double);
    // Get the maximum symbol width
    double maxSymbolWidth() const;

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
    void init();

    virtual void
    drawSymbols(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const;

    virtual void
    drawUserSymbol(QPainter*, SymbolStyle, const QwtOHLCSample&, Qt::Orientation, bool inverted, double symbolWidth) const;

    void drawBar(QPainter*, const QwtOHLCSample&, Qt::Orientation, bool inverted, double width) const;

    void drawCandleStick(QPainter*, const QwtOHLCSample&, Qt::Orientation, double width) const;

    virtual double scaledSymbolWidth(const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;

private:
    QWT_DECLARE_PRIVATE(QwtPlotTradingCurve)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotTradingCurve::PaintAttributes)

#endif
