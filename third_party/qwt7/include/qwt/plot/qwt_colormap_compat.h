/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_COLORMAP_COMPAT_H
#define QWT_COLORMAP_COMPAT_H

#include "qwt_colormap.h"
#include "qwt_interval.h"

/**
 * @brief QwtInterval-based convenience wrappers for QwtColorMap.
 * @details These inline functions provide backward-compatible QwtInterval overloads
 *          for the color map methods. The core QwtColorMap uses (vMin, vMax, value)
 *          signatures to avoid depending on QwtInterval.
 */
namespace QwtColorMapCompat
{

/// Map a value of a given interval into a RGB value.
inline QRgb rgb(const QwtColorMap& map, const QwtInterval& interval, double value)
{
    return map.rgb(interval.minValue(), interval.maxValue(), value);
}

/// Map a value of a given interval into a color index.
inline uint colorIndex(const QwtColorMap& map, int numColors, const QwtInterval& interval, double value)
{
    return map.colorIndex(numColors, interval.minValue(), interval.maxValue(), value);
}

/// Map a value of a given interval into a QColor.
inline QColor color(const QwtColorMap& map, const QwtInterval& interval, double value)
{
    return map.color(interval.minValue(), interval.maxValue(), value);
}

}  // namespace QwtColorMapCompat

#endif
