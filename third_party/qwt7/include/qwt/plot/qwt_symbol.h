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

#ifndef QWT_SYMBOL_H
#define QWT_SYMBOL_H

#include "qwt_global.h"

#include <qpolygon.h>
#include <qpen.h>
#include <qbrush.h>

class QPainter;
class QSize;
class QBrush;
class QPen;
class QColor;
class QPointF;
class QPainterPath;
class QPixmap;
class QByteArray;
class QwtGraphic;

/**
 * @brief A class for drawing symbols
 */
class QWT_EXPORT QwtSymbol
{
public:
    /**
     * @brief Symbol Style
     * @sa setStyle(), style()
     */
    enum Style
    {
        //! No Style. The symbol cannot be drawn.
        NoSymbol = -1,

        //! Ellipse or circle
        Ellipse,

        //! Rectangle
        Rect,

        //! Diamond
        Diamond,

        //! Triangle pointing upwards
        Triangle,

        //! Triangle pointing downwards
        DTriangle,

        //! Triangle pointing upwards
        UTriangle,

        //! Triangle pointing left
        LTriangle,

        //! Triangle pointing right
        RTriangle,

        //! Cross (+)
        Cross,

        //! Diagonal cross (X)
        XCross,

        //! Horizontal line
        HLine,

        //! Vertical line
        VLine,

        //! X combined with +
        Star1,

        //! Six-pointed star
        Star2,

        //! Hexagon
        Hexagon,

        /**
         * The symbol is represented by a painter path, where the
         * origin ( 0, 0 ) of the path coordinate system is mapped to
         * the position of the symbol.
         * @sa setPath(), path()
         */
        Path,

        /**
         * The symbol is represented by a pixmap. The pixmap is centered
         * or aligned to its pin point.
         * @sa setPinPoint()
         */
        Pixmap,

        /**
         * The symbol is represented by a graphic. The graphic is centered
         * or aligned to its pin point.
         * @sa setPinPoint()
         */
        Graphic,

        /**
         * The symbol is represented by a SVG graphic. The graphic is centered
         * or aligned to its pin point.
         * @sa setPinPoint()
         */
        SvgDocument,

        /**
         * Styles >= QwtSymbol::UserSymbol are reserved for derived
         * classes of QwtSymbol that overload drawSymbols() with
         * additional application specific symbol types.
         */
        UserStyle = 1000
    };

    /**
     * @brief Cache policy for symbol rendering
     *
     * Depending on the render engine and the complexity of the
     * symbol shape it might be faster to render the symbol
     * to a pixmap and to paint this pixmap.
     *
     * F.e. the raster paint engine is a pure software renderer
     * where in cache mode a draw operation usually ends in
     * raster operation with the the backing store, that are usually
     * faster, than the algorithms for rendering polygons.
     * But the opposite can be expected for graphic pipelines
     * that can make use of hardware acceleration.
     *
     * The default setting is AutoCache
     *
     * @sa setCachePolicy(), cachePolicy()
     *
     * @note The policy has no effect, when the symbol is painted
     *       to a vector graphics format ( PDF, SVG ).
     * @warning Since Qt 4.8 raster is the default backend on X11
     */
    enum CachePolicy
    {
        //! Don't use a pixmap cache
        NoCache,

        //! Always use a pixmap cache
        Cache,

        /**
         * Use a cache when one of the following conditions is true:
         * - The symbol is rendered with the software renderer ( QPaintEngine::Raster )
         */
        AutoCache
    };

public:
    // Constructor with style
    explicit QwtSymbol(Style = NoSymbol);
    // Constructor with style, brush, pen and size
    QwtSymbol(Style, const QBrush&, const QPen&, const QSize&);
    // Constructor with painter path, brush and pen
    QwtSymbol(const QPainterPath&, const QBrush&, const QPen&);

    // Destructor
    virtual ~QwtSymbol();

    // Set cache policy
    void setCachePolicy(CachePolicy);
    // Get cache policy
    CachePolicy cachePolicy() const;

    // Set size
    void setSize(const QSize&);
    // Set size with width and height
    void setSize(int width, int height = -1);
    // Get size
    const QSize& size() const;

    // Set pin point
    void setPinPoint(const QPointF& pos, bool enable = true);
    // Get pin point
    QPointF pinPoint() const;

    // Enable/disable pin point
    void setPinPointEnabled(bool);
    // Check if pin point is enabled
    bool isPinPointEnabled() const;

    // Set color
    virtual void setColor(const QColor&);

    // Set brush
    void setBrush(const QBrush&);
    // Get brush
    const QBrush& brush() const;

    // Set pen with color, width and style
    void setPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    // Set pen
    void setPen(const QPen&);
    // Get pen
    const QPen& pen() const;

    // Set style
    void setStyle(Style);
    // Get style
    Style style() const;

    // Set painter path
    void setPath(const QPainterPath&);
    // Get painter path
    const QPainterPath& path() const;

    // Set pixmap
    void setPixmap(const QPixmap&);
    // Get pixmap
    const QPixmap& pixmap() const;

    // Set graphic
    void setGraphic(const QwtGraphic&);
    // Get graphic
    const QwtGraphic& graphic() const;

#ifndef QWT_NO_SVG
    // Set SVG document
    void setSvgDocument(const QByteArray&);
#endif

    // Draw symbol in rectangle
    void drawSymbol(QPainter*, const QRectF&) const;
    // Draw symbol at point
    void drawSymbol(QPainter*, const QPointF&) const;
    // Draw symbols at polygon points
    void drawSymbols(QPainter*, const QPolygonF&) const;
    // Draw symbols at points
    void drawSymbols(QPainter*, const QPointF*, int numPoints) const;

    // Get bounding rectangle
    virtual QRect boundingRect() const;
    // Invalidate cache
    void invalidateCache();

protected:
    virtual void renderSymbols(QPainter*, const QPointF*, int numPoints) const;

private:
    QwtSymbol(const QwtSymbol&)            = delete;
    QwtSymbol& operator=(const QwtSymbol&) = delete;
    QWT_DECLARE_PRIVATE(QwtSymbol)
};

/**
 * @brief Draw the symbol at a specified position
 * @param painter Painter
 * @param pos Position of the symbol in screen coordinates
 * @sa drawSymbols()
 *
 */
inline void QwtSymbol::drawSymbol(QPainter* painter, const QPointF& pos) const
{
    drawSymbols(painter, &pos, 1);
}

/**
 * @brief Draw symbols at the specified points
 * @param painter Painter
 * @param points Positions of the symbols in screen coordinates
 * @sa drawSymbol()
 *
 */
inline void QwtSymbol::drawSymbols(QPainter* painter, const QPolygonF& points) const
{
    drawSymbols(painter, points.data(), points.size());
}

#endif
