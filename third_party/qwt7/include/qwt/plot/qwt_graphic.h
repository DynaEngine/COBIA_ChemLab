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

#ifndef QWT_GRAPHIC_H
#define QWT_GRAPHIC_H

#include "qwt_global.h"
#include "qwt_null_paintdevice.h"
#include "qwt_painter_command.h"

#include <qmetatype.h>

/**
 * @brief A paint device for scalable graphics
 *
 * @details QwtGraphic is the representation of a graphic that is tailored for
 * scalability. Like QPicture it will be initialized by QPainter
 * operations and can be replayed later to any target paint device.
 *
 * While the usual image representations QImage and QPixmap are not
 * scalable Qt offers two paint devices, that might be candidates
 * for representing a vector graphic:
 *
 * - QPicture: Unfortunately QPicture had been forgotten, when Qt4
 *   introduced floating point based render engines. Its API
 *   is still on integers, what make it unusable for proper scaling.
 *
 * - QSvgRenderer/QSvgGenerator: Unfortunately QSvgRenderer hides too
 *   much information about its nodes in internal APIs, that are necessary
 *   for proper layout calculations. Also it is derived from QObject and
 *   can't be copied like QImage/QPixmap.
 *
 * QwtGraphic maps all scalable drawing primitives to a QPainterPath
 * and stores them together with the painter state changes
 * ( pen, brush, transformation ... ) in a list of QwtPaintCommands.
 * For being a complete QPaintDevice it also stores pixmaps or images,
 * what is somehow against the idea of the class, because these objects
 * can't be scaled without a loss in quality.
 *
 * The main issue about scaling a QwtGraphic object are the pens used for
 * drawing the outlines of the painter paths. While non cosmetic pens
 * ( QPen::isCosmetic() ) are scaled with the same ratio as the path,
 * cosmetic pens have a fixed width. A graphic might have paths with
 * different pens - cosmetic and non-cosmetic.
 *
 * QwtGraphic caches 2 different rectangles:
 *
 * - control point rectangle: The control point rectangle is the bounding
 *   rectangle of all control point rectangles of the painter paths,
 *   or the target rectangle of the pixmaps/images.
 *
 * - bounding rectangle: The bounding rectangle extends the control point
 *   rectangle by what is needed for rendering the outline with an unscaled pen.
 *
 * Because the offset for drawing the outline depends on the shape
 * of the painter path ( the peak of a triangle is different than the flat side )
 * scaling with a fixed aspect ratio always needs to be calculated from the
 * control point rectangle.
 *
 */
class QWT_EXPORT QwtGraphic : public QwtNullPaintDevice
{
public:
    /**
     * @brief Hint how to render a graphic
     *
     */
    enum RenderHint
    {
        /**
         * When rendering a QwtGraphic a specific scaling between
         * the controlPointRect() and the coordinates of the target rectangle
         * is set up internally in render().
         *
         * When RenderPensUnscaled is set this specific scaling is applied
         * for the control points only, but not for the pens.
         * All other painter transformations ( set up by application code )
         * are supposed to work like usual.
         *
         */
        RenderPensUnscaled = 0x1
    };

    Q_DECLARE_FLAGS(RenderHints, RenderHint)

    /**
     * @brief Indicator if the graphic contains a specific type of painter command
     *
     */
    enum CommandType
    {
        //! The graphic contains scalable vector data
        VectorData = 1 << 0,

        //! The graphic contains raster data ( QPixmap or QImage )
        RasterData = 1 << 1,

        //! The graphic contains transformations beyond simple translations
        Transformation = 1 << 2
    };

    Q_DECLARE_FLAGS(CommandTypes, CommandType)

    // Constructor
    QwtGraphic();
    // Copy constructor
    QwtGraphic(const QwtGraphic&);

    // Destructor
    ~QwtGraphic() override;

    // Assignment operator
    QwtGraphic& operator=(const QwtGraphic&);

    // Clear all stored commands
    void reset();

    // Check if the graphic is null
    bool isNull() const;
    // Check if the graphic is empty
    bool isEmpty() const;

    // Get the types of painter commands being used
    CommandTypes commandTypes() const;

    // Replay all recorded painter commands
    void render(QPainter*) const;

    // Render graphic scaled to fit into given size
    void render(QPainter*, const QSizeF&, Qt::AspectRatioMode = Qt::IgnoreAspectRatio) const;

    // Render graphic aligned to a position
    void render(QPainter*, const QPointF&, Qt::Alignment = Qt::AlignTop | Qt::AlignLeft) const;

    // Render graphic scaled to fit into given rectangle
    void render(QPainter*, const QRectF&, Qt::AspectRatioMode = Qt::IgnoreAspectRatio) const;

    // Convert graphic to QPixmap in default size
    QPixmap toPixmap(qreal devicePixelRatio = 0.0) const;

    // Convert graphic to QPixmap with specified size
    QPixmap toPixmap(const QSize&, Qt::AspectRatioMode = Qt::IgnoreAspectRatio, qreal devicePixelRatio = 0.0) const;

    // Convert graphic to QImage in default size
    QImage toImage(qreal devicePixelRatio = 0.0) const;

    // Convert graphic to QImage with specified size
    QImage toImage(const QSize&, Qt::AspectRatioMode = Qt::IgnoreAspectRatio, qreal devicePixelRatio = 0.0) const;

    // Calculate the scaled bounding rectangle
    QRectF scaledBoundingRect(qreal sx, qreal sy) const;

    // Get the bounding rectangle
    QRectF boundingRect() const;
    // Get the control point rectangle
    QRectF controlPointRect() const;

    // Get the list of recorded paint commands
    const QVector< QwtPainterCommand >& commands() const;
    // Set the paint commands
    void setCommands(const QVector< QwtPainterCommand >&);

    // Set the default size
    void setDefaultSize(const QSizeF&);
    // Get the default size
    QSizeF defaultSize() const;

    // Get the height for a given width
    qreal heightForWidth(qreal width) const;
    // Get the width for a given height
    qreal widthForHeight(qreal height) const;

    // Set a render hint
    void setRenderHint(RenderHint, bool on = true);
    // Test a render hint
    bool testRenderHint(RenderHint) const;

    // Get the render hints
    RenderHints renderHints() const;

protected:
    virtual QSize sizeMetrics() const override;

    virtual void drawPath(const QPainterPath&) override;

    virtual void drawPixmap(const QRectF&, const QPixmap&, const QRectF&) override;

    virtual void drawImage(const QRectF&, const QImage&, const QRectF&, Qt::ImageConversionFlags) override;

    virtual void updateState(const QPaintEngineState&) override;

private:
    void renderGraphic(QPainter*, QTransform*) const;

    void updateBoundingRect(const QRectF&);
    void updateControlPointRect(const QRectF&);

    class PathInfo;

    QWT_DECLARE_PRIVATE(QwtGraphic)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtGraphic::RenderHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtGraphic::CommandTypes)
Q_DECLARE_METATYPE(QwtGraphic)

#endif
