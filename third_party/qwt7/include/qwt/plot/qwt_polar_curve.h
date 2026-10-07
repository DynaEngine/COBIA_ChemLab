/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_CURVE_H
#define QWT_POLAR_CURVE_H

#include "qwt_global.h"
#include "qwt_polar_item.h"
#include "qwt_point_polar.h"
#include "qwt_series_data.h"

class QPainter;
class QwtSymbol;
class QwtCurveFitter;

/**
 * @brief An item, that represents a series of points
 * @details A curve is the representation of a series of points in polar coordinates.
 *          The points are connected to the curve using the abstract QwtData interface.
 *
 * @sa QwtPolarPlot, QwtSymbol, QwtScaleMap
 */
class QWT_EXPORT QwtPolarCurve : public QwtPolarItem
{
public:
    /**
     * @brief Curve styles
     * @sa setStyle(), style()
     */
    enum CurveStyle
    {
        /// Don't draw a curve. Note: This doesn't affect the symbols.
        NoCurve,

        /**
         * Connect the points with straight lines. The lines might
         * be interpolated depending on the 'Fitted' attribute. Curve
         * fitting can be configured using setCurveFitter().
         */
        Lines,

        /// Values > 100 are reserved for user specific curve styles
        UserCurve = 100
    };

    /**
     * @brief Attributes how to represent the curve on the legend
     * @details If none of the flags is activated QwtPlotCurve tries to find
     *          a color representing the curve and paints a rectangle with it.
     *          In the default setting all attributes are off.
     * @sa setLegendAttribute(), testLegendAttribute()
     */
    enum LegendAttribute
    {
        /**
         * If the curveStyle() is not NoCurve a line is painted with the curvePen().
         */
        LegendShowLine = 0x01,

        /**
         * If the curve has a valid symbol it is painted.
         */
        LegendShowSymbol = 0x02
    };

    Q_DECLARE_FLAGS(LegendAttributes, LegendAttribute)

    /// Constructor
    explicit QwtPolarCurve();
    /// Constructor with title
    explicit QwtPolarCurve(const QwtText& title);
    /// Constructor with title string
    explicit QwtPolarCurve(const QString& title);

    /// Destructor
    ~QwtPolarCurve() override;

    /// Get the runtime type information
    virtual int rtti() const override;

    /// Set a legend attribute
    void setLegendAttribute(LegendAttribute, bool on = true);
    /// Test a legend attribute
    bool testLegendAttribute(LegendAttribute) const;

    /// Set the data
    void setData(QwtSeriesData< QwtPointPolar >* data);
    /// Get the data
    const QwtSeriesData< QwtPointPolar >* data() const;

    /// Get the data size
    size_t dataSize() const;
    /// Get a sample
    QwtPointPolar sample(int i) const;

    /// Set the pen
    void setPen(const QPen&);
    /// Get the pen
    const QPen& pen() const;

    /// Set the curve style
    void setStyle(CurveStyle style);
    /// Get the curve style
    CurveStyle style() const;

    /// Set the symbol
    void setSymbol(QwtSymbol*);
    /// Get the symbol
    const QwtSymbol* symbol() const;

    /// Set the curve fitter
    void setCurveFitter(QwtCurveFitter*);
    /// Get the curve fitter
    QwtCurveFitter* curveFitter() const;

    /// Draw the curve
    virtual void draw(QPainter* p,
                      const QwtScaleMap& azimuthMap,
                      const QwtScaleMap& radialMap,
                      const QPointF& pole,
                      double radius,
                      const QRectF& canvasRect) const override;

    /// Draw the curve from index to index
    virtual void
    draw(QPainter* p, const QwtScaleMap& azimuthMap, const QwtScaleMap& radialMap, const QPointF& pole, int from, int to) const;

    /// Get the bounding interval for a scale
    virtual QwtInterval boundingInterval(int scaleId) const override;

    /// Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

protected:
    /// Initialize the curve
    void init();

    /// Draw the curve
    virtual void drawCurve(QPainter*,
                           int style,
                           const QwtScaleMap& azimuthMap,
                           const QwtScaleMap& radialMap,
                           const QPointF& pole,
                           int from,
                           int to) const;

    /// Draw the symbols
    virtual void drawSymbols(QPainter*,
                             const QwtSymbol&,
                             const QwtScaleMap& azimuthMap,
                             const QwtScaleMap& radialMap,
                             const QPointF& pole,
                             int from,
                             int to) const;

    /// Draw the lines
    void
    drawLines(QPainter*, const QwtScaleMap& azimuthMap, const QwtScaleMap& radialMap, const QPointF& pole, int from, int to) const;

private:
    QwtSeriesData< QwtPointPolar >* m_series;

    QWT_DECLARE_PRIVATE(QwtPolarCurve)
};

//! @return the the curve data
inline const QwtSeriesData< QwtPointPolar >* QwtPolarCurve::data() const
{
    return m_series;
}

/*!
    @param i index
    @return point at position i
 */
inline QwtPointPolar QwtPolarCurve::sample(int i) const
{
    return m_series->sample(i);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPolarCurve::LegendAttributes)

#endif
