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

#ifndef QWT_MATH_H
#define QWT_MATH_H
// stl
#include <iterator>
#include <type_traits>
#include <algorithm>
// qt
#include <QPointF>
#include <QtMath>
// qwt
#include "qwtcore_global.h"
/*
   Microsoft says:

   Define _USE_MATH_DEFINES before including math.h to expose these macro
   definitions for common math constants.  These are placed under an #ifdef
   since these commonly-defined names are not part of the C/C++ standards.
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#define undef_USE_MATH_DEFINES
#endif

#include <cmath>

#ifdef undef_USE_MATH_DEFINES
#undef _USE_MATH_DEFINES
#undef undef_USE_MATH_DEFINES
#endif

#ifndef M_E
#define M_E (2.7182818284590452354)
#endif

#ifndef M_LOG2E
#define M_LOG2E (1.4426950408889634074)
#endif

#ifndef M_LOG10E
#define M_LOG10E (0.43429448190325182765)
#endif

#ifndef M_LN2
#define M_LN2 (0.69314718055994530942)
#endif

#ifndef M_LN10
#define M_LN10 (2.30258509299404568402)
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef M_PI_2
#define M_PI_2 (1.57079632679489661923)
#endif

#ifndef M_PI_4
#define M_PI_4 (0.78539816339744830962)
#endif

#ifndef M_1_PI
#define M_1_PI (0.31830988618379067154)
#endif

#ifndef M_2_PI
#define M_2_PI (0.63661977236758134308)
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI (1.12837916709551257390)
#endif

#ifndef M_SQRT2
#define M_SQRT2 (1.41421356237309504880)
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 (0.70710678118654752440)
#endif

#if defined(QT_WARNING_PUSH)
/*
    early Qt versions not having QT_WARNING_PUSH is full of warnings
    so that we do not care of suppressing those from below
 */
QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wdouble-promotion")
QT_WARNING_DISABLE_GCC("-Wdouble-promotion")
#endif

/*
    On systems, where qreal is a float you often run into
    compiler issues with qMin/qMax.
 */

//! @return Minimum of a and b.
constexpr inline float qwtMinF(float a, float b)
{
    return (a < b) ? a : b;
}

//! @return Minimum of a and b.
constexpr inline double qwtMinF(double a, double b)
{
    return (a < b) ? a : b;
}

//! @return Minimum of a and b.
constexpr inline qreal qwtMinF(float a, double b)
{
    return (a < b) ? a : b;
}

//! @return Minimum of a and b.
constexpr inline qreal qwtMinF(double a, float b)
{
    return (a < b) ? a : b;
}

//! @return Maximum of a and b.
constexpr inline float qwtMaxF(float a, float b)
{
    return (a < b) ? b : a;
}

//! @return Maximum of a and b.
constexpr inline double qwtMaxF(double a, double b)
{
    return (a < b) ? b : a;
}

//! @return Maximum of a and b.
constexpr inline qreal qwtMaxF(float a, double b)
{
    return (a < b) ? b : a;
}

//! @return Maximum of a and b.
constexpr inline qreal qwtMaxF(double a, float b)
{
    return (a < b) ? b : a;
}

#if defined(QT_WARNING_POP)
QT_WARNING_POP
#endif

QWTCORE_EXPORT double qwtNormalizeRadians(double radians);
QWTCORE_EXPORT double qwtNormalizeDegrees(double degrees);
QWTCORE_EXPORT quint32 qwtRand();

/*!
   @brief Compare 2 values, relative to an interval
   @details Values are "equal", when:
          value2 - value1 <= abs(intervalSize * 10e^{-6})
   @param value1 First value to compare
   @param value2 Second value to compare
   @param intervalSize interval size
   @return 0: if equal, -1: if value2 > value1, 1: if value1 > value2
 */
inline int qwtFuzzyCompare(double value1, double value2, double intervalSize)
{
    const double eps = qAbs(1.0e-6 * intervalSize);

    if (value2 - value1 > eps)
        return -1;

    if (value1 - value2 > eps)
        return 1;

    return 0;
}

//! Return the sign
inline int qwtSign(double x)
{
    if (x > 0.0)
        return 1;
    else if (x < 0.0)
        return (-1);
    else
        return 0;
}

//! Return the square of a number
inline constexpr double qwtSqr(double x)
{
    return x * x;
}

//! Approximation of arc tangent (error below 0.005 radians)
inline double qwtFastAtan(double x)
{
    if (x < -1.0)
        return -M_PI_2 - x / (x * x + 0.28);

    if (x > 1.0)
        return M_PI_2 - x / (x * x + 0.28);

    return x / (1.0 + x * x * 0.28);
}

//! Approximation of arc tangent 2 (error below 0.005 radians)
inline double qwtFastAtan2(double y, double x)
{
    if (x > 0)
        return qwtFastAtan(y / x);

    if (x < 0) {
        const double d = qwtFastAtan(y / x);
        return (y >= 0) ? d + M_PI : d - M_PI;
    }

    if (y < 0.0)
        return -M_PI_2;

    if (y > 0.0)
        return M_PI_2;

    return 0.0;
}

/**
 * @brief Calculate a value of a cubic polynomial
 * @param[in] x Value
 * @param[in] a Cubic coefficient
 * @param[in] b Quadratic coefficient
 * @param[in] c Linear coefficient
 * @param[in] d Constant offset
 * @return Value of the polynomial for x
 */
inline constexpr double qwtCubicPolynomial(double x, double a, double b, double c, double d)
{
    return (((a * x) + b) * x + c) * x + d;
}

//! Translate degrees into radians
inline constexpr double qwtRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

//! Translate radians into degrees
inline constexpr double qwtDegrees(double degrees)
{
    return degrees * 180.0 / M_PI;
}

/*!
    @brief The same as qCeil, but avoids including qmath.h
    @return Ceiling of value.
 */
inline int qwtCeil(qreal value)
{
    using std::ceil;
    return int(ceil(value));
}
/*!
    @brief The same as qFloor, but avoids including qmath.h
    @return Floor of value.
 */
inline int qwtFloor(qreal value)
{
    using std::floor;
    return int(floor(value));
}

/**
 * @brief Verify and adjust array index range
 * @details Ensures the given index range is within valid bounds and returns the actual number of valid elements.
 *          This function automatically corrects invalid index values to ensure i1 <= i2.
 *          Typically used for array or container range checking to prevent out-of-bounds access.
 * @param size Total size of the array
 * @param i1 Start index (will be corrected to valid range)
 * @param i2 End index (will be corrected to valid range)
 * @return Number of valid elements in range, or 0 if array size is invalid
 */
inline int qwtVerifyRange(int size, int& i1, int& i2)
{
    if (size < 1)
        return 0;

    i1 = qBound(0, i1, size - 1);
    i2 = qBound(0, i2, size - 1);

    if (i1 > i2)
        qSwap(i1, i2);

    return (i2 - i1 + 1);
}

/**
 * @brief Calculate distance between two points
 * @param p1 First point
 * @param p2 Second point
 * @return Distance between p1 and p2
 */
inline double qwtDistance(const QPointF& p1, const QPointF& p2)
{
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    return qSqrt(dx * dx + dy * dy);
}

/**
 * @brief Check if floating point value is NaN or infinity
 * @details This function checks whether a floating point value is either NaN (Not a Number)
 *          or infinite (positive or negative infinity). It uses std::isfinite() for the check.
 * @tparam T Floating point type (float, double, long double)
 * @param value The value to check
 * @return true if value is NaN or infinite
 * @return false if value is finite
 * @note This overload is enabled only for floating point types
 * @see std::isfinite()
 */
template< typename T >
inline typename std::enable_if< std::is_floating_point< T >::value, bool >::type qwt_is_nan_or_inf(const T& value)
{
    return !std::isfinite(value);
}

/**
 * @brief Check if QPointF contains NaN or infinite coordinates
 * @details This function checks whether either the x or y coordinate of a QPointF
 *          is NaN (Not a Number) or infinite. Both coordinates are checked.
 * @param point The QPointF to check
 * @return true if either x or y coordinate is NaN or infinite
 * @return false if both coordinates are finite
 */
inline bool qwt_is_nan_or_inf(const QPointF& point)
{
    return !std::isfinite(point.x()) || !std::isfinite(point.y());
}

// Default check function - for other types
/**
 * @brief Default check function for non-floating point types
 * @details Always returns false for non-floating point types as they cannot be NaN or infinite.
 */
template< typename T >
typename std::enable_if< !std::is_floating_point< T >::value && !std::is_same< T, QPointF >::value,
                         bool >::type inline qwt_is_nan_or_inf(const T& /*value*/)
{
    return false;
}

/**
 * @brief Check if iterator range contains NaN or Inf values
 * @details Checks whether elements in the iterator range [first, last) contain NaN or infinity values.
 *          Supports floating point types, QPointF, etc.
 * @tparam InputIt Input iterator type
 * @param first Start iterator of the range
 * @param last End iterator of the range
 * @return true if range contains at least one NaN or Inf value, false otherwise
 */
template< typename InputIt >
inline bool qwtContainsNanOrInf(InputIt first, InputIt last)
{
    // Iterate using iterator, calling the appropriate qwt_is_nan_or_inf function for each element
    for (InputIt it = first; it != last; ++it) {
        if (qwt_is_nan_or_inf(*it)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief In-place clean array, removing NaN and Inf values
 * @details Uses std::remove_if algorithm to move elements to keep to the front of the container,
 *          then erases the unwanted elements.
 * @tparam Container Container type
 * @param container Container to clean (modified in place)
 * @return Number of removed elements
 */
template< typename Container >
inline std::size_t qwtRemoveNanOrInf(Container& container)
{
    // Use std::remove_if algorithm to move retained elements to the front of the container
    auto new_end = std::remove_if(container.begin(), container.end(), [](const typename Container::value_type& value) {
        return qwt_is_nan_or_inf(value);
    });

    // Calculate the number of removed elements
    std::size_t removed_count = std::distance(new_end, container.end());

    // Actually erase the unwanted elements
    if (removed_count > 0) {
        container.erase(new_end, container.end());
    }

    return removed_count;
}

/**
 * @brief Remove all NaN or Inf values from container (returns new container)
 * @details Creates a new container with only finite values copied from the source.
 * @tparam Container Container type
 * @param container Container to process
 * @return New container without NaN or Inf values
 */
template< typename Container >
inline Container qwtRemoveNanOrInfCopy(const Container& container)
{
    Container result;
    result.reserve(container.size());  // pre-allocate for efficiency

    // Copy only elements that are not NaN or Inf
    std::copy_if(container.begin(),
                 container.end(),
                 std::back_inserter(result),
                 [](const typename Container::value_type& value) { return !qwt_is_nan_or_inf(value); });

    return result;
}

/**
 * @brief Compare two floating-point ranges for fuzzy equality using qFuzzyCompare
 * @details Works with any container as long as iterators meet ForwardIterator requirements.
 *          If the lengths differ it returns false immediately; otherwise it performs
 *          element-wise qFuzzyCompare and returns true only if all comparisons succeed.
 * @tparam It1 Type of the first range's iterator
 * @tparam It2 Type of the second range's iterator
 * @param[in] first1 Begin iterator of the first range
 * @param[in] last1 End iterator of the first range
 * @param[in] first2 Begin iterator of the second range
 * @param[in] last2 End iterator of the second range
 * @return true if ranges have equal length and all corresponding elements satisfy qFuzzyCompare
 * @return false if lengths differ or any element pair fails qFuzzyCompare
 * @note Element type must be acceptable to qFuzzyCompare (usually float/double)
 * @sa qFuzzyCompare
 */
template< typename It1, typename It2 >
bool fuzzyRangeEqual(It1 first1, It1 last1, It2 first2, It2 last2)
{
    // 1. Different lengths => not equal
    if (std::distance(first1, last1) != std::distance(first2, last2))
        return false;

    // 2. Element-wise qFuzzyCompare
    for (; first1 != last1; ++first1, ++first2) {
        if (!qFuzzyCompare(*first1, *first2))
            return false;
    }
    return true;
}

#endif
