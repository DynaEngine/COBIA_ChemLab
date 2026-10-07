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

#ifndef QWT_PLOT_CURVE_3D_H
#define QWT_PLOT_CURVE_3D_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"

class QwtColorMap;

/**
 * @brief Curve that displays 3D points as dots, where the z coordinate is mapped to a color
 * @details QwtPlotSpectroCurve displays 3D points as dots, with the z coordinate mapped to a color
 *          using a color map.
 *
 */
class QWT_EXPORT QwtPlotSpectroCurve : public QwtPlotSeriesItem, public QwtSeriesStore< QwtPoint3D >
{
public:
    /**
     * @brief Paint attributes
     *
     */
    enum PaintAttribute
    {
        /// Clip points outside the canvas rectangle
        ClipPoints = 1
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    /// Constructor
    explicit QwtPlotSpectroCurve(const QString& title = QString());
    /// Constructor with title
    explicit QwtPlotSpectroCurve(const QwtText& title);

    /// Destructor
    ~QwtPlotSpectroCurve() override;

    /// Get the runtime type information
    virtual int rtti() const override;

    /// Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    /// Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    /// Set the samples
    void setSamples(const QVector< QwtPoint3D >&);
    /// Set the samples
    void setSamples(QwtSeriesData< QwtPoint3D >*);

    /// Set the color map
    void setColorMap(QwtColorMap*);
    /// Get the color map
    const QwtColorMap* colorMap() const;

    /// Set the color range
    void setColorRange(const QwtInterval&);
    /// Get the color range
    const QwtInterval& colorRange() const;

    /// Draw the series
    virtual void drawSeries(QPainter*,
                            const QwtScaleMap& xMap,
                            const QwtScaleMap& yMap,
                            const QRectF& canvasRect,
                            int from,
                            int to) const override;

    /// Set the pen width
    void setPenWidth(double);
    /// Get the pen width
    double penWidth() const;

protected:
    /// Draw the dots
    virtual void
    drawDots(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const;

private:
    /// Initialize the spectro curve
    void init();

    QWT_DECLARE_PRIVATE(QwtPlotSpectroCurve)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotSpectroCurve::PaintAttributes)

#endif
