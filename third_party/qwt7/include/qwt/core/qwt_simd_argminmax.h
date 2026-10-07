#ifndef QWT_SIMD_ARGMINMAX_H
#define QWT_SIMD_ARGMINMAX_H

#include "qwtcore_global.h"

#include <cfloat>

/*!
  @brief Result of SIMD-accelerated argmin + argmax search.
  @details Contains local indices (0-based relative to input pointer)
           and values of the minimum and maximum elements found.
           For all-NaN input arrays, min_val = DBL_MAX, max_val = -DBL_MAX.
 */
struct QwtArgMinMaxResult
{
    int minIdx;
    int maxIdx;
    double minVal;
    double maxVal;
};

/*!
  @brief Find argmin and argmax in a double array using SIMD acceleration.
  @details Uses runtime CPU feature detection to select the optimal
           implementation: AVX2 -> SSE4.2 -> Scalar fallback.
           NaN values are naturally ignored via IEEE 754 comparison semantics.
  @param data Pointer to double array (must not be null)
  @param count Number of elements (must be >= 1)
  @return QwtArgMinMaxResult with min/max indices and values
  @note For all-NaN arrays, returns {0, 0, DBL_MAX, -DBL_MAX}
  @note Preconditions: data != nullptr && count >= 1
 */
QWTCORE_EXPORT QwtArgMinMaxResult qwtSimdArgMinMax(const double* data, int count);

#endif
