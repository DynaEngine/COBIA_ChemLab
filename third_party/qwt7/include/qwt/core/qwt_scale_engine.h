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

#ifndef QWT_SCALE_ENGINE_H
#define QWT_SCALE_ENGINE_H

#include "qwtcore_global.h"
#include "qwt_scale_div.h"

class QwtInterval;
class QwtTransform;

/**
 * @brief Arithmetic including a tolerance
 */
class QWTCORE_EXPORT QwtScaleArithmetic
{
public:
    /// Ceiling with epsilon tolerance
    static double ceilEps(double value, double intervalSize);
    /// Floor with epsilon tolerance
    static double floorEps(double value, double intervalSize);

    /// Divide with epsilon tolerance
    static double divideEps(double intervalSize, double numSteps);

    /// Divide interval
    static double divideInterval(double intervalSize, int numSteps, uint base);
};

/**
 * @brief Base class for scale engines
 * @details A scale engine tries to find "reasonable" ranges and step sizes
 *          for scales. The layout of the scale can be varied with setAttribute().
 *          Qwt offers implementations for logarithmic and linear scales.
 * @sa QwtLinearScaleEngine, QwtLogScaleEngine
 */
class QWTCORE_EXPORT QwtScaleEngine
{
public:
    /**
     * @brief Layout attributes
     * @sa setAttribute(), testAttribute(), reference(), lowerMargin(), upperMargin()
     */
    enum Attribute
    {
        /// @brief No attributes
        NoAttribute = 0x00,

        /// @brief Build a scale which includes the reference() value
        IncludeReference = 0x01,

        /// @brief Build a scale which is symmetric to the reference() value
        Symmetric = 0x02,

        /**
         * The endpoints of the scale are supposed to be equal the
         * outmost included values plus the specified margins (see setMargins()).
         * If this attribute is *not* set, the endpoints of the scale will
         * be integer multiples of the step size.
         */
        Floating = 0x04,

        /// @brief Turn the scale upside down
        Inverted = 0x08
    };

    Q_DECLARE_FLAGS(Attributes, Attribute)

    explicit QwtScaleEngine(uint base = 10);
    virtual ~QwtScaleEngine();

    /// Set the base
    void setBase(uint base);
    /// @return the base
    uint base() const;

    /// Set an attribute
    void setAttribute(Attribute, bool on = true);
    /// @return true if an attribute is set
    bool testAttribute(Attribute) const;

    /// Set attributes
    void setAttributes(Attributes);
    /// @return the attributes
    Attributes attributes() const;

    /// Set the reference value
    void setReference(double);
    /// @return the reference value
    double reference() const;

    /// Set the margins
    void setMargins(double lower, double upper);
    /// @return the lower margin
    double lowerMargin() const;
    /// @return the upper margin
    double upperMargin() const;

    /// Align and divide an interval
    virtual void autoScale(int maxNumSteps, double& x1, double& x2, double& stepSize) const = 0;

    /// Calculate a scale division
    virtual QwtScaleDiv
    divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize = 0.0) const = 0;

    /// Set the transformation
    void setTransformation(QwtTransform*);
    /// @return the transformation
    QwtTransform* transformation() const;

protected:
    /// Check if an interval contains a value
    bool contains(const QwtInterval&, double value) const;
    /// Strip values outside an interval
    QList< double > strip(const QList< double >&, const QwtInterval&) const;

    /// Divide an interval
    double divideInterval(double intervalSize, int numSteps) const;

    /// Build an interval around a value
    QwtInterval buildInterval(double value) const;

private:
    QwtScaleEngine(const QwtScaleEngine&)            = delete;
    QwtScaleEngine& operator=(const QwtScaleEngine&) = delete;

    QWT_DECLARE_PRIVATE(QwtScaleEngine)
};

/**
 * @brief A scale engine for linear scales
 * @details The step size will fit into the pattern \f$\left\{ 1,2,5\right\} \cdot 10^{n}\f$, where n is an integer.
 */

class QWTCORE_EXPORT QwtLinearScaleEngine : public QwtScaleEngine
{
public:
    explicit QwtLinearScaleEngine(uint base = 10);
    ~QwtLinearScaleEngine() override;

    virtual void autoScale(int maxNumSteps, double& x1, double& x2, double& stepSize) const override;

    virtual QwtScaleDiv
    divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize = 0.0) const override;

protected:
    QwtInterval align(const QwtInterval&, double stepSize) const;

    void buildTicks(const QwtInterval&, double stepSize, int maxMinorSteps, QList< double > ticks[ QwtScaleDiv::NTickTypes ]) const;

    QList< double > buildMajorTicks(const QwtInterval& interval, double stepSize) const;

    void buildMinorTicks(const QList< double >& majorTicks,
                         int maxMinorSteps,
                         double stepSize,
                         QList< double >& minorTicks,
                         QList< double >& mediumTicks) const;
};

/**
 * @brief A scale engine for logarithmic scales
 * @details The step size is measured in *decades* and the major step size will be adjusted
 *          to fit the pattern \f$\left\{ 1,2,3,5\right\} \cdot 10^{n}\f$, where n is a natural
 *          number including zero.
 *
 * @warning The step size as well as the margins are measured in *decades*.
 */

class QWTCORE_EXPORT QwtLogScaleEngine : public QwtScaleEngine
{
public:
    explicit QwtLogScaleEngine(uint base = 10);
    ~QwtLogScaleEngine() override;

    virtual void autoScale(int maxNumSteps, double& x1, double& x2, double& stepSize) const override;

    virtual QwtScaleDiv
    divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize = 0.0) const override;

protected:
    QwtInterval align(const QwtInterval&, double stepSize) const;

    void buildTicks(const QwtInterval&, double stepSize, int maxMinorSteps, QList< double > ticks[ QwtScaleDiv::NTickTypes ]) const;

    QList< double > buildMajorTicks(const QwtInterval& interval, double stepSize) const;

    void buildMinorTicks(const QList< double >& majorTicks,
                         int maxMinorSteps,
                         double stepSize,
                         QList< double >& minorTicks,
                         QList< double >& mediumTicks) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtScaleEngine::Attributes)

#endif
