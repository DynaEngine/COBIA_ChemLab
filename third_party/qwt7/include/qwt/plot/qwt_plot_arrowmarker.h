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

#ifndef QWT_PLOT_ARROWMARKER_H
#define QWT_PLOT_ARROWMARKER_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

class QString;
class QRectF;
class QwtText;
class QPainterPath;
class QPen;
class QBrush;

/**
 * @brief A class for drawing arrow markers on plots
 * @details QwtPlotArrowMarker is a specialized plot item for drawing arrows
 *          with customizable start and end points, arrow styles, lengths,
 *          and colors. The arrow is drawn in canvas pixel coordinates,
 *          maintaining its size during zoom operations while following
 *          canvas panning.
 *
 *          Key features:
 *          - Support for explicit start and end point specification
 *          - Arrow positioning via start point, length, and rotation angle
 *          - Customizable arrow head and tail styles
 *          - Canvas pixel-based drawing (size preserved during zoom)
 *          - Flexible styling system with predefined and custom shapes
 *
 *
 */

class QWT_EXPORT QwtPlotArrowMarker : public QwtPlotItem
{
public:
    /**
     * @brief Arrow endpoint style types
     * @sa setHeadStyle(), setTailStyle(), headStyle(), tailStyle()
     *
     */
    enum EndpointStyle
    {
        /**
         * @brief  No endpoint (invisible)
         *
         */
        NoEndpoint = 0,

        /**
         * @brief Arrow head style
         *
         */
        ArrowHead,

        /**
         * @brief Circle endpoint
         *
         */
        Circle,

        /**
         * @brief Square endpoint
         *
         */
        Square,

        /**
         * @brief Diamond endpoint
         *
         */
        Diamond,

        /**
         * @brief Triangle endpoint
         *
         */
        Triangle,

        /**
         * @brief Custom QPainterPath endpoint
         *
         */
        CustomPath
    };

    /**
     * @brief Arrow positioning mode
     * @sa setPositionMode(), positionMode()
     *
     */
    enum PositionMode
    {
        /**
         * @brief  Use explicit start and end points
         *
         * In this mode, the arrow's start and end points are specified in plot coordinates.
         * The arrow's size will change proportionally to the plot's zoom level.
         *
         */
        ExplicitPoints,

        /**
         * @brief  start point, length, and angle
         *
         * in this mode, the arrow length will be fixed regardless of the zoom level
         *
         */
        StartLengthAngle
    };

    // Constructor
    explicit QwtPlotArrowMarker();

    // Constructor with title
    explicit QwtPlotArrowMarker(const QString& title);

    // Constructor with QwtText title
    explicit QwtPlotArrowMarker(const QwtText& title);

    // Destructor
    ~QwtPlotArrowMarker() override;

    // Get the runtime type information
    virtual int rtti() const override;

    // Position and geometry methods

    // Get the start point
    QPointF startPoint() const;

    // Get the end point
    QPointF endPoint() const;

    // Set the start point
    void setStartPoint(const QPointF& point);

    // Set the end point
    void setEndPoint(const QPointF& point);

    // Set both start and end points
    void setPoints(const QPointF& start, const QPointF& end);

    // Get the arrow length in pixels
    double length() const;

    // Set the arrow length in pixels
    void setLength(double length);

    // Get the rotation angle in degrees
    double angle() const;

    // Set the rotation angle in degrees
    void setAngle(double angle);

    // Get the positioning mode
    PositionMode positionMode() const;

    // Set the positioning mode
    void setPositionMode(PositionMode mode);

    // Style and appearance methods

    // Get the arrow line pen
    const QPen& linePen() const;

    // Set the arrow line pen
    void setLinePen(const QPen& pen);

    // Convenience method to set line color and width
    void setLinePen(const QColor& color, qreal width = 1.0, Qt::PenStyle style = Qt::SolidLine);

    // Get the head style
    EndpointStyle headStyle() const;

    // Set the head style
    void setHeadStyle(EndpointStyle style);

    // Get the tail style
    EndpointStyle tailStyle() const;

    // Set the tail style
    void setTailStyle(EndpointStyle style);

    // Get the head size in pixels
    QSizeF headSize() const;

    // Set the head size in pixels
    void setHeadSize(const QSizeF& size);

    // Convenience method to set head size with equal width and height
    void setHeadSize(qreal size);

    // Get the tail size in pixels
    QSizeF tailSize() const;

    // Set the tail size in pixels
    void setTailSize(const QSizeF& size);

    // Convenience method to set tail size with equal width and height
    void setTailSize(qreal size);

    // Get the head brush
    const QBrush& headBrush() const;

    // Set the head brush
    void setHeadBrush(const QBrush& brush);

    // Get the tail brush
    const QBrush& tailBrush() const;

    // Set the tail brush
    void setTailBrush(const QBrush& brush);

    // Get the head pen
    const QPen& headPen() const;

    // Set the head pen
    void setHeadPen(const QPen& pen);

    // Get the tail pen
    const QPen& tailPen() const;

    // Set the tail pen
    void setTailPen(const QPen& pen);

    // Set a custom path for head endpoint
    void setHeadCustomPath(const QPainterPath& path);

    // Get the custom head path
    QPainterPath headCustomPath() const;

    // Set a custom path for tail endpoint
    void setTailCustomPath(const QPainterPath& path);

    // Get the custom tail path
    QPainterPath tailCustomPath() const;

    // Drawing methods

    // Draw the arrow marker
    virtual void
    draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    // Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF& size) const override;

protected:
    /// Draw the arrow line
    virtual void drawArrowLine(QPainter* painter, const QPointF& canvasStart, const QPointF& canvasEnd) const;
    /// Draw a cached endpoint path
    virtual void drawCachedEndpoint(QPainter* painter,
                                    const QPointF& position,
                                    const QPainterPath& cachedPath,
                                    const QSizeF& size,
                                    const QPen& pen,
                                    const QBrush& brush,
                                    double rotation = 0.0) const;

    /// Convert plot coordinates to canvas coordinates
    virtual QPointF toCanvasPoint(const QPointF& plotPoint, const QwtScaleMap& xMap, const QwtScaleMap& yMap) const;

    /// Calculate end point based on start point, length, and angle
    virtual QPointF calculateEndPoint() const;

private:
    // Disable copy constructor and assignment operator
    QwtPlotArrowMarker(const QwtPlotArrowMarker&);
    QwtPlotArrowMarker& operator=(const QwtPlotArrowMarker&);

    QWT_DECLARE_PRIVATE(QwtPlotArrowMarker)
};

#endif  // QWT_PLOT_ARROWMARKER_H