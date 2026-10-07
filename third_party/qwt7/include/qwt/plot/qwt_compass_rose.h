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

#ifndef QWT_COMPASS_ROSE_H
#define QWT_COMPASS_ROSE_H

#include "qwt_global.h"
#include <qpalette.h>

class QPainter;

/**
 *   @brief Abstract base class for a compass rose
 *   @details QwtCompassRose is the abstract base class that defines the interface
 *            for drawing compass roses in QwtCompass.
 */
class QWT_EXPORT QwtCompassRose
{
public:
    // Constructor
    QwtCompassRose();
    // Destructor
    virtual ~QwtCompassRose();

    // Sets the palette for the rose
    virtual void setPalette(const QPalette&);
    // Returns the palette of the rose
    const QPalette& palette() const;

    /**
     *   @brief Draw the rose
     *   @param[in] painter Painter
     *   @param[in] center Center point
     *   @param[in] radius Radius of the rose
     *   @param[in] north Position pointing north
     *   @param[in] colorGroup Color group
     */
    virtual void draw(QPainter* painter,
                      const QPointF& center,
                      double radius,
                      double north,
                      QPalette::ColorGroup colorGroup = QPalette::Active) const = 0;

private:
    QwtCompassRose(const QwtCompassRose&)            = delete;
    QwtCompassRose& operator=(const QwtCompassRose&) = delete;

    QPalette m_palette;
};

/**
 *   @brief A simple rose for QwtCompass
 *   @details QwtSimpleCompassRose provides a simple compass rose implementation
 *            with configurable thorn count and levels.
 */
class QWT_EXPORT QwtSimpleCompassRose : public QwtCompassRose
{
public:
    // Constructs a simple compass rose with specified number of thorns and levels
    QwtSimpleCompassRose(int numThorns = 8, int numThornLevels = -1);
    // Destructor
    ~QwtSimpleCompassRose() override;

    // Sets the width of the rose heads (range: 0.03 to 0.4)
    void setWidth(double);
    // Returns the width of the rose heads
    double width() const;

    // Sets the number of thorns on one level (aligned to multiple of 4, minimum 4)
    void setNumThorns(int);
    // Returns the number of thorns
    int numThorns() const;

    // Sets the number of thorn levels
    void setNumThornLevels(int);
    // Returns the number of thorn levels
    int numThornLevels() const;

    // Sets the shrink factor for thorns with each level (default: 0.9)
    void setShrinkFactor(double factor);
    // Returns the shrink factor for thorns
    double shrinkFactor() const;

    // Set flat style
    void setFlatStyle(bool);
    // Return flat style
    bool flatStyle() const;

    // Draw the rose (override from base class)
    virtual void
    draw(QPainter*, const QPointF& center, double radius, double north, QPalette::ColorGroup = QPalette::Active) const override;

    // Static helper to draw a rose with specified parameters
    static void drawRose(QPainter*,
                         const QPalette&,
                         const QPointF& center,
                         double radius,
                         double north,
                         double width,
                         int numThorns,
                         int numThornLevels,
                         double shrinkFactor,
                         bool flatStyle = true);

private:
    QWT_DECLARE_PRIVATE(QwtSimpleCompassRose)
};

#endif
