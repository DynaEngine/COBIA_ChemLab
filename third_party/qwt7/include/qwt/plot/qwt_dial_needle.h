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

#ifndef QWT_DIAL_NEEDLE_H
#define QWT_DIAL_NEEDLE_H

#include "qwt_global.h"
#include <qpalette.h>

class QPainter;

/**
 * @brief Base class for needles that can be used in a QwtDial
 * @details QwtDialNeedle is a pointer that indicates a value by pointing
 *          to a specific direction.
 * @sa QwtDial, QwtCompass
 */

class QWT_EXPORT QwtDialNeedle
{
public:
    /// Constructor
    QwtDialNeedle();
    /// Destructor
    virtual ~QwtDialNeedle();

    /// Set the palette for the needle
    virtual void setPalette(const QPalette&);
    /// Return the palette of the needle
    const QPalette& palette() const;

    /// Set flat style
    void setFlatStyle(bool);
    /// Return flat style
    bool flatStyle() const;

    /// Draw the needle
    virtual void
    draw(QPainter*, const QPointF& center, double length, double direction, QPalette::ColorGroup = QPalette::Active) const;

protected:
    /*!
       @brief Draw the needle

       The origin of the needle is at position (0.0, 0.0 )
       pointing in direction 0.0 ( = east ).

       The painter is already initialized with translation and
       rotation.

       @param painter Painter
       @param length Length of the needle
       @param colorGroup Color group, used for painting

       @sa setPalette(), palette()
     */
    virtual void drawNeedle(QPainter* painter, double length, QPalette::ColorGroup colorGroup) const = 0;

    virtual void drawKnob(QPainter*, double width, const QBrush&, bool sunken) const;

protected:
    QPalette m_palette;
    bool m_flatStyle;

private:
    QwtDialNeedle(const QwtDialNeedle&)            = delete;
    QwtDialNeedle& operator=(const QwtDialNeedle&) = delete;
};

/**
 * @brief A needle for dial widgets
 * @details The following colors are used:
 *          - QPalette::Mid: Pointer
 *          - QPalette::Base: Knob
 * @sa QwtDial, QwtCompass
 */

class QWT_EXPORT QwtDialSimpleNeedle : public QwtDialNeedle
{
public:
    /**
     * @brief Style of the needle
     */
    enum Style
    {
        // Arrow style
        Arrow,

        // A straight line from the center
        Ray
    };

    /// Constructor
    QwtDialSimpleNeedle(Style, bool hasKnob = true, const QColor& mid = Qt::gray, const QColor& base = Qt::darkGray);

    /// Set the width of the needle
    void setWidth(double width);
    /// Return the width of the needle
    double width() const;

protected:
    virtual void drawNeedle(QPainter*, double length, QPalette::ColorGroup) const override;

private:
    Style m_style;
    bool m_hasKnob;
    double m_width;
};

/**
 * @brief A magnet needle for compass widgets
 * @details A magnet needle points to two opposite directions indicating
 *          north and south.
 *          The following colors are used:
 *          - QPalette::Light: Used for pointing south
 *          - QPalette::Dark: Used for pointing north
 *          - QPalette::Base: Knob (ThinStyle only)
 * @sa QwtDial, QwtCompass
 */

class QWT_EXPORT QwtCompassMagnetNeedle : public QwtDialNeedle
{
public:
    /**
     * @brief Style of the needle
     */
    enum Style
    {
        // A needle with a triangular shape
        TriangleStyle,

        // A thin needle
        ThinStyle
    };

    /// Constructor
    QwtCompassMagnetNeedle(Style = TriangleStyle, const QColor& light = Qt::white, const QColor& dark = Qt::red);

protected:
    virtual void drawNeedle(QPainter*, double length, QPalette::ColorGroup) const override;

private:
    Style m_style;
};

/**
 * @brief An indicator for the wind direction
 * @details QwtCompassWindArrow shows the direction where the wind comes from.
 *          The following colors are used:
 *          - QPalette::Light: Used for Style1, or the light half of Style2
 *          - QPalette::Dark: Used for the dark half of Style2
 * @sa QwtDial, QwtCompass
 */

class QWT_EXPORT QwtCompassWindArrow : public QwtDialNeedle
{
public:
    /**
     * @brief Style of the arrow
     */
    enum Style
    {
        // A needle pointing to the center
        Style1,

        // A needle pointing to the center
        Style2
    };

    /// Constructor
    QwtCompassWindArrow(Style, const QColor& light = Qt::white, const QColor& dark = Qt::gray);

protected:
    virtual void drawNeedle(QPainter*, double length, QPalette::ColorGroup) const override;

private:
    Style m_style;
};

#endif
