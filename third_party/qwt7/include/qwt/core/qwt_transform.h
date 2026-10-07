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

#ifndef QWT_TRANSFORM_H
#define QWT_TRANSFORM_H

#include "qwtcore_global.h"

/**
 * @brief A transformation between coordinate systems
 * @details QwtTransform manipulates values when being mapped between
 *          the scale and the paint device coordinate system.
 *          A transformation consists of 2 methods:
 *          - transform
 *          - invTransform
 *          where one is the inverse function of the other.
 *          When p1, p2 are the boundaries of the paint device coordinates
 *          and s1, s2 the boundaries of the scale, QwtScaleMap uses the
 *          following calculations:
 *          - p = p1 + ( p2 - p1 ) * ( T( s ) - T( s1 ) / ( T( s2 ) - T( s1 ) );
 *          - s = invT ( T( s1 ) + ( T( s2 ) - T( s1 ) ) * ( p - p1 ) / ( p2 - p1 ) );
 */
class QWTCORE_EXPORT QwtTransform
{
public:
    //! Constructor
    QwtTransform();
    //! Destructor
    virtual ~QwtTransform();

    //! Modify value to be a valid value for the transformation
    virtual double bounded(double value) const;
    //! Transformation function
    virtual double transform(double value) const = 0;
    //! Inverse transformation function
    virtual double invTransform(double value) const = 0;
    //! Virtualized copy operation
    virtual QwtTransform* copy() const = 0;

private:
    QwtTransform(const QwtTransform&)            = delete;
    QwtTransform& operator=(const QwtTransform&) = delete;
};

/**
 * @brief Null transformation
 * @details QwtNullTransform returns the values unmodified.
 */
class QWTCORE_EXPORT QwtNullTransform : public QwtTransform
{
public:
    //! Constructor
    QwtNullTransform();
    //! Destructor
    ~QwtNullTransform() override;

    //! Transformation function - returns value unmodified
    virtual double transform(double value) const override;
    //! Inverse transformation function - returns value unmodified
    virtual double invTransform(double value) const override;
    //! Clone of the transformation
    virtual QwtTransform* copy() const override;
};
/**
 * @brief Logarithmic transformation
 * @details QwtLogTransform modifies the values using log() and exp().
 * @note In the calculations of QwtScaleMap the base of the log function
 *       has no effect on the mapping. So QwtLogTransform can be used
 *       for log2(), log10() or any other logarithmic scale.
 */
class QWTCORE_EXPORT QwtLogTransform : public QwtTransform
{
public:
    //! Constructor
    QwtLogTransform();
    //! Destructor
    ~QwtLogTransform() override;

    //! Transformation function - log(value)
    virtual double transform(double value) const override;
    //! Inverse transformation function - exp(value)
    virtual double invTransform(double value) const override;
    //! Bounded function - qBound(LogMin, value, LogMax)
    virtual double bounded(double value) const override;
    //! Clone of the transformation
    virtual QwtTransform* copy() const override;

    //! Smallest allowed value for logarithmic scales: 1.0e-150
    static const double LogMin;
    //! Largest allowed value for logarithmic scales: 1.0e150
    static const double LogMax;
};

/**
 * @brief A transformation using pow()
 * @details QwtPowerTransform preserves the sign of a value.
 *          F.e. a transformation with a factor of 2
 *          transforms a value of -3 to -9 and v.v. Thus QwtPowerTransform
 *          can be used for scales including negative values.
 */
class QWTCORE_EXPORT QwtPowerTransform : public QwtTransform
{
public:
    //! Constructor with exponent parameter
    explicit QwtPowerTransform(double exponent);
    //! Destructor
    ~QwtPowerTransform() override;

    //! Transformation function - pow() preserving sign
    virtual double transform(double value) const override;
    //! Inverse transformation function - inverse pow() preserving sign
    virtual double invTransform(double value) const override;
    //! Clone of the transformation
    virtual QwtTransform* copy() const override;

private:
    const double m_exponent;
};

#endif
