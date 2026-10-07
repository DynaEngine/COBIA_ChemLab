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

#ifndef QWT_SPLINE_H
#define QWT_SPLINE_H

#include "qwt_global.h"
#include "qwt_spline.h"

class QwtSplineParametrization;
class QwtSplinePolynomial;
class QPainterPath;
class QLineF;
class QPolygonF;

#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief Base class for all splines
 *
 * A spline is a curve represented by a sequence of polynomials. Spline approximation
 * is the process of finding polynomials for a given set of points.
 * When the algorithm preserves the initial points it is called interpolating.
 *
 * Splines can be classified according to conditions of the polynomials that
 * are met at the start/endpoints of the pieces:
 *
 * - Geometric Continuity
 *   - G0: polynomials are joined
 *   - G1: first derivatives are proportional at the join point
 *         The curve tangents thus have the same direction, but not necessarily the
 *         same magnitude. i.e., C1'(1) = (a,b,c) and C2'(0) = (k*a, k*b, k*c).
 *   - G2: first and second derivatives are proportional at join point
 *
 * - Parametric Continuity
 *   - C0: curves are joined
 *   - C1: first derivatives equal
 *   - C2: first and second derivatives are equal
 *
 * Geometric continuity requires the geometry to be continuous, while parametric
 * continuity requires that the underlying parameterization be continuous as well.
 * Parametric continuity of order n implies geometric continuity of order n,
 * but not vice-versa.
 *
 * QwtSpline is the base class for spline approximations of any continuity.
 */
class QWT_EXPORT QwtSpline
{
public:
    /**
     * @brief Boundary type specifying the spline at its endpoints
     *
     * @sa setBoundaryType(), boundaryType()
     */
    enum BoundaryType
    {
        /**
         * The polynomials at the start/endpoint depend on specific conditions
         *
         * @sa QwtSpline::BoundaryCondition
         */
        ConditionalBoundaries,

        /**
         * The polynomials at the start/endpoint are found by using
         * imaginary additional points. Additional points at the end
         * are found by translating points from the beginning or v.v.
         */
        PeriodicPolygon,

        /**
         * ClosedPolygon is similar to PeriodicPolygon beside, that
         * the interpolation includes the connection between the last
         * and the first control point.
         *
         * @note Only works for parametrizations, where the parameter increment
         *      for the the final closing line is positive.
         *      This excludes QwtSplineParametrization::ParameterX and
         *      QwtSplineParametrization::ParameterY
         */

        ClosedPolygon
    };

    /**
     * @brief position of a boundary condition
     * @sa boundaryCondition(), boundaryValue()
     */
    enum BoundaryPosition
    {
        //! the condition is at the beginning of the polynomial
        AtBeginning,

        //! the condition is at the end of the polynomial
        AtEnd
    };

    /**
     * @brief Boundary condition
     *
     * A spline algorithm calculates polynomials by looking
     * a couple of points back/ahead ( locality() ). At the ends
     * additional rules are necessary to compensate the missing
     * points.
     *
     * @sa boundaryCondition(), boundaryValue()
     * @sa QwtSplineC2::BoundaryConditionC2
     */
    enum BoundaryCondition
    {
        /**
         * The first derivative at the end point is given
         * @sa boundaryValue()
         */
        Clamped1,

        /**
         * The second derivative at the end point is given
         *
         * @sa boundaryValue()
         * @note a condition having a second derivative of 0
         *      is also called "natural".
         */
        Clamped2,

        /**
         * The third derivative at the end point is given
         *
         * @sa boundaryValue()
         * @note a condition having a third derivative of 0
         *      is also called "parabolic runout".
         */
        Clamped3,

        /**
         * The first derivate at the endpoint is related to the first derivative
         * at its neighbour by the boundary value. F,e when the boundary
         * value at the end is 1.0 then the slope at the last 2 points is
         * the same.
         *
         * @sa boundaryValue().
         */
        LinearRunout
    };

    //! Constructor
    QwtSpline();
    //! Destructor
    virtual ~QwtSpline();

    //! Set parametrization by type
    void setParametrization(int type);
    //! Set parametrization object
    void setParametrization(QwtSplineParametrization*);
    //! Get parametrization
    const QwtSplineParametrization* parametrization() const;

    //! Set boundary type
    void setBoundaryType(BoundaryType);
    //! Get boundary type
    BoundaryType boundaryType() const;

    //! Set boundary value
    void setBoundaryValue(BoundaryPosition, double value);
    //! Get boundary value
    double boundaryValue(BoundaryPosition) const;

    //! Set boundary condition
    void setBoundaryCondition(BoundaryPosition, int condition);
    //! Get boundary condition
    int boundaryCondition(BoundaryPosition) const;

    //! Set boundary conditions for both ends
    void setBoundaryConditions(int condition, double valueBegin = 0.0, double valueEnd = 0.0);

    //! Get polygon approximation with tolerance
    virtual QPolygonF polygon(const QPolygonF&, double tolerance) const;
    //! Get painter path from polygon (pure virtual)
    virtual QPainterPath painterPath(const QPolygonF&) const = 0;

    //! Get locality (number of points used for calculation)
    virtual uint locality() const;

private:
    QwtSpline(const QwtSpline&)            = delete;
    QwtSpline& operator=(const QwtSpline&) = delete;

    QWT_DECLARE_PRIVATE(QwtSpline)
};

/**
 * @brief Base class for spline interpolation
 *
 * Spline interpolation is the process of interpolating a set of points
 * piecewise with polynomials. The initial set of points is preserved.
 */
class QWT_EXPORT QwtSplineInterpolating : public QwtSpline
{
public:
    QwtSplineInterpolating();
    ~QwtSplineInterpolating() override;

    virtual QPolygonF equidistantPolygon(const QPolygonF&, double distance, bool withNodes) const;

    virtual QPolygonF polygon(const QPolygonF&, double tolerance) const override;

    virtual QPainterPath painterPath(const QPolygonF&) const override;
    virtual QVector< QLineF > bezierControlLines(const QPolygonF&) const = 0;

private:
    QwtSplineInterpolating(const QwtSplineInterpolating&)            = delete;
    QwtSplineInterpolating& operator=(const QwtSplineInterpolating&) = delete;
};

/**
 * @brief Base class for spline interpolations with G1 (first order geometric) continuity
 *
 * Provides first order geometric continuity (G1) between adjoining curves.
 */
class QWT_EXPORT QwtSplineG1 : public QwtSplineInterpolating
{
public:
    QwtSplineG1();
    ~QwtSplineG1() override;
};

/**
 * @brief Base class for spline interpolations with C1 (first order parametric) continuity
 *
 * All interpolations with C1 continuity are based on rules for finding
 * the first derivative at some control points.
 *
 * For non-parametric splines those points are the curve points, while
 * for parametric splines the calculation is done twice using a parameter value t.
 *
 * @sa QwtSplineParametrization
 */
class QWT_EXPORT QwtSplineC1 : public QwtSplineG1
{
public:
    QwtSplineC1();
    ~QwtSplineC1() override;

    virtual QPainterPath painterPath(const QPolygonF&) const override;
    virtual QVector< QLineF > bezierControlLines(const QPolygonF&) const override;

    virtual QPolygonF equidistantPolygon(const QPolygonF&, double distance, bool withNodes) const override;

    // these methods are the non parametric part
    virtual QVector< QwtSplinePolynomial > polynomials(const QPolygonF&) const;
    virtual QVector< double > slopes(const QPolygonF&) const = 0;

    virtual double slopeAtBeginning(const QPolygonF&, double slopeNext) const;
    virtual double slopeAtEnd(const QPolygonF&, double slopeBefore) const;
};

/**
 * @brief Base class for spline interpolations with C2 (second order parametric) continuity
 *
 * All interpolations with C2 continuity are based on rules for finding
 * the second derivative at some control points.
 *
 * For non-parametric splines those points are the curve points, while
 * for parametric splines the calculation is done twice using a parameter value t.
 *
 * @sa QwtSplineParametrization
 */
class QWT_EXPORT QwtSplineC2 : public QwtSplineC1
{
public:
    /*!
       Boundary condition that requires C2 continuity

       @sa QwtSpline::boundaryCondition, QwtSpline::BoundaryCondition
     */
    enum BoundaryConditionC2
    {
        /*!
           The second derivate at the endpoint is related to the second derivatives
           at the 2 neighbours: cv[0] := 2.0 * cv[1] - cv[2].

           @note boundaryValue() is ignored
         */
        CubicRunout = LinearRunout + 1,

        /*!
           The 3rd derivate at the endpoint matches the 3rd derivate at its neighbours.
           Or in other words: the first/last curve segment extents the polynomial of its
           neighboured polynomial

           @note boundaryValue() is ignored
         */
        NotAKnot
    };

    QwtSplineC2();
    ~QwtSplineC2() override;

    virtual QPainterPath painterPath(const QPolygonF&) const override;
    virtual QVector< QLineF > bezierControlLines(const QPolygonF&) const override;

    virtual QPolygonF equidistantPolygon(const QPolygonF&, double distance, bool withNodes) const override;

    // calculating the parametric equations
    virtual QVector< QwtSplinePolynomial > polynomials(const QPolygonF&) const override;
    virtual QVector< double > slopes(const QPolygonF&) const override;
    virtual QVector< double > curvatures(const QPolygonF&) const = 0;
};

#endif
