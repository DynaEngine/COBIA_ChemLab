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

#ifndef QWT_PLOT_INTERVAL_CURVE_H
#define QWT_PLOT_INTERVAL_CURVE_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"

class QwtIntervalSymbol;
template< typename T >
class QwtSeriesData;

/**
 * @brief QwtPlotIntervalCurve represents a series of samples, where each value
 *        is associated with an interval ( \f$[y1,y2] = f(x)\f$ )
 * @details The representation depends on the style() and an optional symbol()
 *          that is displayed for each interval. QwtPlotIntervalCurve might be used
 *          to display error bars or the area between 2 curves.
 *
 */
class QWT_EXPORT QwtPlotIntervalCurve : public QwtPlotSeriesItem, public QwtSeriesStore< QwtIntervalSample >
{
public:
    /**
     * @brief Curve styles
     * @details The default setting is QwtPlotIntervalCurve::Tube.
     * @sa setStyle(), style()
     *
     */
    enum CurveStyle
    {
        /**
         * Don't draw a curve. Note: This doesn't affect the symbols.
         *
         */
        NoCurve,

        /**
         * Build 2 curves from the upper and lower limits of the intervals
         * and draw them with the pen(). The area between the curves is
         * filled with the brush().
         *
         */
        Tube,

        /**
         * Styles >= QwtPlotIntervalCurve::UserCurve are reserved for derived
         * classes that overload drawSeries() with
         * additional application specific curve types.
         *
         */
        UserCurve = 100
    };

    /**
     * @brief Attributes to modify the drawing algorithm
     * @sa setPaintAttribute(), testPaintAttribute()
     *
     */
    enum PaintAttribute
    {
        /**
         * Clip polygons before painting them. In situations, where points
         * are far outside the visible area (f.e when zooming deep) this
         * might be a substantial improvement for the painting performance.
         *
         */
        ClipPolygons = 0x01,

        /// Check if a symbol is on the plot canvas before painting it
        ClipSymbol = 0x02
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    // Constructor
    explicit QwtPlotIntervalCurve(const QString& title = QString());

    // Constructor with QwtText title
    explicit QwtPlotIntervalCurve(const QwtText& title);

    // Destructor
    ~QwtPlotIntervalCurve() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Attach the interval curve to a plot (applies color cycle if pen not user-set)
    void attach(QwtPlot* plot) override;

    // Set paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);

    // Test paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set samples from a vector
    void setSamples(const QVector< QwtIntervalSample >&);

    // Set samples from a series data
    void setSamples(QwtSeriesData< QwtIntervalSample >*);

    // Set pen with color, width and style
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);

    // Set pen
    void setPen(const QPen&);

    // Get pen
    const QPen& pen() const;

    // Set brush
    void setBrush(const QBrush&);

    // Get brush
    const QBrush& brush() const;

    // Set curve style
    void setStyle(CurveStyle style);

    // Get curve style
    CurveStyle style() const;

    // Set symbol
    void setSymbol(const QwtIntervalSymbol*);

    // Get symbol
    const QwtIntervalSymbol* symbol() const;

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
     * @brief Initialize the curve
     */
    void init();

    /**
     * @brief Draw the tube
     */
    virtual void
    drawTube(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const;

    /**
     * @brief Draw the symbols
     */
    virtual void drawSymbols(QPainter*,
                             const QwtIntervalSymbol&,
                             const QwtScaleMap& xMap,
                             const QwtScaleMap& yMap,
                             const QRectF& canvasRect,
                             int from,
                             int to) const;

private:
    QWT_DECLARE_PRIVATE(QwtPlotIntervalCurve)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotIntervalCurve::PaintAttributes)

#endif
