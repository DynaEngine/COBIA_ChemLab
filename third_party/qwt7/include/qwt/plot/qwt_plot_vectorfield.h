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

#ifndef QWT_PLOT_VECTOR_FIELD_H
#define QWT_PLOT_VECTOR_FIELD_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"
class QwtVectorFieldSymbol;
class QwtColorMap;
class QPen;
class QBrush;

/**
 * @brief A plot item, that represents a vector field
 * @details A vector field is a representation of a points with a given magnitude and direction
 *          as arrows. While the direction affects the direction of the arrow, the magnitude
 *          might be represented as a color or by the length of the arrow.
 *
 * @sa QwtVectorFieldSymbol, QwtVectorFieldSample
 *
 */
class QWT_EXPORT QwtPlotVectorField : public QwtPlotSeriesItem, public QwtSeriesStore< QwtVectorFieldSample >
{
public:
    /**
     * @brief Indicator origin
     * @details Depending on the origin the indicator symbol ( usually an arrow )
     *          will be to the position of the corresponding sample.
     *
     */
    enum IndicatorOrigin
    {
        /// symbol points to the sample position
        OriginHead,

        /// The arrow starts at the sample position
        OriginTail,

        /// The arrow is centered at the sample position
        OriginCenter
    };

    /**
     * @brief Paint attributes
     * @details Attributes to modify the rendering
     * @sa setPaintAttribute(), testPaintAttribute()
     *
     */
    enum PaintAttribute
    {
        /**
         * FilterVectors calculates an average sample from all samples
         * that lie in the same cell of a grid that is determined by
         * setting the rasterSize().
         *
         * @sa setRasterSize()
         *
         */
        FilterVectors = 0x01
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    /**
     * @brief Magnitude mode
     * @details Depending on the MagnitudeMode the magnitude component will have
     *          an impact on the attributes of the symbol/arrow.
     *
     * @sa setMagnitudeMode()
     *
     */
    enum MagnitudeMode
    {
        /**
         * The magnitude will be mapped to a color using a color map
         * @sa magnitudeRange(), colorMap()
         *
         */
        MagnitudeAsColor = 0x01,

        /**
         * The magnitude will have an impact on the length of the arrow/symbol
         * @sa arrowLength(), magnitudeScaleFactor()
         *
         */
        MagnitudeAsLength = 0x02
    };

    Q_DECLARE_FLAGS(MagnitudeModes, MagnitudeMode)

    // Constructor
    explicit QwtPlotVectorField(const QString& title = QString());
    // Constructor with title
    explicit QwtPlotVectorField(const QwtText& title);

    // Destructor
    ~QwtPlotVectorField() override;

    // Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set a magnitude mode
    void setMagnitudeMode(MagnitudeMode, bool on = true);
    // Test a magnitude mode
    bool testMagnitudeMode(MagnitudeMode) const;

    // Set the symbol
    void setSymbol(QwtVectorFieldSymbol*);
    // Get the symbol
    const QwtVectorFieldSymbol* symbol() const;

    // Set the pen
    void setPen(const QPen&);
    // Get the pen
    QPen pen() const;

    // Set the brush
    void setBrush(const QBrush&);
    // Get the brush
    QBrush brush() const;

    // Set the raster size
    void setRasterSize(const QSizeF&);
    // Get the raster size
    QSizeF rasterSize() const;

    // Set the indicator origin
    void setIndicatorOrigin(IndicatorOrigin);
    // Get the indicator origin
    IndicatorOrigin indicatorOrigin() const;

    // Set the samples
    void setSamples(const QVector< QwtVectorFieldSample >&);
    // Set the samples
    void setSamples(QwtVectorFieldData*);

    // Set the color map
    void setColorMap(QwtColorMap*);
    // Get the color map
    const QwtColorMap* colorMap() const;

    // Set the magnitude range
    void setMagnitudeRange(const QwtInterval&);
    // Get the magnitude range
    QwtInterval magnitudeRange() const;

    // Set the minimum arrow length
    void setMinArrowLength(double);
    // Get the minimum arrow length
    double minArrowLength() const;

    // Set the maximum arrow length
    void setMaxArrowLength(double);
    // Get the maximum arrow length
    double maxArrowLength() const;

    // Calculate the arrow length for a given magnitude
    virtual double arrowLength(double magnitude) const;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    // Draw the series
    virtual void drawSeries(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            int from,
                            int to) const override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

    // Set the magnitude scale factor
    void setMagnitudeScaleFactor(double factor);
    // Get the magnitude scale factor
    double magnitudeScaleFactor() const;

protected:
    virtual void
    drawSymbols(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const;

    virtual void drawSymbol(QPainter*, double x, double y, double vx, double vy) const;

    virtual void dataChanged() override;

private:
    void init();

    QWT_DECLARE_PRIVATE(QwtPlotVectorField)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotVectorField::PaintAttributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotVectorField::MagnitudeModes)

#endif
