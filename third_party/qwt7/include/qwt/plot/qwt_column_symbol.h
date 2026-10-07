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

#ifndef QWT_COLUMN_SYMBOL_H
#define QWT_COLUMN_SYMBOL_H

#include "qwt_global.h"
#include "qwt_interval.h"

#include <qnamespace.h>

class QPainter;
class QPalette;
class QRectF;

/**
 * @brief Directed rectangle representing bounding rectangle and orientation of a column
 * @details QwtColumnRect is a data structure that stores the horizontal and vertical
 *          intervals defining a column's extent, along with a direction that indicates
 *          how the column should be drawn. It is used by QwtColumnSymbol to determine
 *          the column's orientation and boundaries.
 */
class QWT_EXPORT QwtColumnRect
{
public:
    //! Direction of the column
    enum Direction
    {
        //! From left to right
        LeftToRight,

        //! From right to left
        RightToLeft,

        //! From bottom to top
        BottomToTop,

        //! From top to bottom
        TopToBottom
    };

    /**
     * @brief Build a rectangle with invalid intervals directed BottomToTop
     */
    QwtColumnRect() : direction(BottomToTop)
    {
    }

    //! Return a normalized QRect built from the intervals
    QRectF toRect() const;

    //! Return Orientation
    Qt::Orientation orientation() const
    {
        if (direction == LeftToRight || direction == RightToLeft)
            return Qt::Horizontal;

        return Qt::Vertical;
    }

    //! Interval for the horizontal coordinates
    QwtInterval hInterval;

    //! Interval for the vertical coordinates
    QwtInterval vInterval;

    //! Direction
    Direction direction;
};

/**
 * @brief A drawing primitive for columns
 * @details QwtColumnSymbol defines how columns are rendered in column-based charts
 *          like bar charts. It provides different styles (Box, etc.) and frame styles
 *          (Plain, Raised, NoFrame) to customize the appearance of column graphics.
 *          The symbol can be extended by deriving new types with UserStyle.
 */
class QWT_EXPORT QwtColumnSymbol
{
public:
    /*!
       @brief Style
       @sa setStyle(), style()
     */
    enum Style
    {
        //! No Style, the symbol draws nothing
        NoStyle = -1,

        /*!
           The column is painted with a frame depending on the frameStyle() and lineWidth() using the palette().
         */
        Box,

        /*!
           Styles >= QwtColumnSymbol::UserStyle are reserved for derived classes of QwtColumnSymbol
           that overload draw() with additional application specific symbol types.
         */
        UserStyle = 1000
    };

    /*!
       @brief Frame Style used in Box style()
       @sa Style, setFrameStyle(), frameStyle(), setStyle(), setPalette()
     */
    enum FrameStyle
    {
        //! No frame
        NoFrame,

        //! A plain frame style
        Plain,

        //! A raised frame style
        Raised
    };

public:
    //! Constructor with style parameter
    explicit QwtColumnSymbol(Style = NoStyle);
    //! Destructor
    virtual ~QwtColumnSymbol();

    //! Set the frame style
    void setFrameStyle(FrameStyle);
    //! Return the frame style
    FrameStyle frameStyle() const;

    //! Set the line width for the frame
    void setLineWidth(int width);
    //! Return the line width
    int lineWidth() const;

    //! Set the column style
    void setStyle(Style);
    //! Return the column style
    Style style() const;

    //! Set the outline pen
    void setPen(const QPen& pen);
    //! Return the outline pen
    QPen pen() const;

    //! Set the fill brush
    void setBrush(const QBrush& b);
    //! Return the fill brush
    QBrush brush() const;

    //! Draw the column symbol
    virtual void draw(QPainter*, const QwtColumnRect&) const;

protected:
    void drawBox(QPainter*, const QwtColumnRect&) const;

private:
    QwtColumnSymbol(const QwtColumnSymbol&)            = delete;
    QwtColumnSymbol& operator=(const QwtColumnSymbol&) = delete;

    QWT_DECLARE_PRIVATE(QwtColumnSymbol)
};

#endif
