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

#ifndef QWT_PLOT_RASTERITEM_H
#define QWT_PLOT_RASTERITEM_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

#include <qstring.h>

class QwtInterval;

/**
 * @brief A class, which displays raster data
 * @details Raster data is a grid of pixel values, that can be represented
 *          as a QImage. It is used for many types of information like
 *          spectrograms, cartograms, geographical maps ...
 *
 *          Often a plot has several types of raster data organized in layers.
 *          (e.g a geographical map, with weather statistics ).
 *          Using setAlpha() raster items can be stacked easily.
 *
 *          QwtPlotRasterItem is only implemented for images of the following formats:
 *          QImage::Format_Indexed8, QImage::Format_ARGB32.
 *
 * @sa QwtPlotSpectrogram
 *
 */

class QWT_EXPORT QwtPlotRasterItem : public QwtPlotItem
{
public:
    /**
     * @brief Cache policy
     * @details The default policy is NoCache
     *
     */
    enum CachePolicy
    {
        /**
         * renderImage() is called each time the item has to be repainted
         *
         */
        NoCache,

        /**
         * renderImage() is called, whenever the image cache is not valid,
         * or the scales, or the size of the canvas has changed.
         *
         * This type of cache is useful for improving the performance
         * of hide/show operations or manipulations of the alpha value.
         * All other situations are handled by the canvas backing store.
         *
         */
        PaintCache
    };

    /**
     * @brief Paint attributes
     * @details Attributes to modify the drawing algorithm.
     * @sa setPaintAttribute(), testPaintAttribute()
     *
     */
    enum PaintAttribute
    {
        /**
         * When the image is rendered according to the data pixels
         * ( QwtRasterData::pixelHint() ) it can be expanded to paint
         * device resolution before it is passed to QPainter.
         * The expansion algorithm rounds the pixel borders in the same
         * way as the axis ticks, what is usually better than the
         * scaling algorithm implemented in Qt.
         * Disabling this flag might make sense, to reduce the size of a
         * document/file. If this is possible for a document format
         * depends on the implementation of the specific QPaintEngine.
         *
         */
        PaintInDeviceResolution = 1
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    // Constructor
    explicit QwtPlotRasterItem(const QString& title = QString());
    // Constructor with title
    explicit QwtPlotRasterItem(const QwtText& title);
    // Destructor
    ~QwtPlotRasterItem() override;

    // Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set the alpha value
    void setAlpha(int alpha);
    // Get the alpha value
    int alpha() const;

    // Set the cache policy
    void setCachePolicy(CachePolicy);
    // Get the cache policy
    CachePolicy cachePolicy() const;

    // Invalidate the cache
    void invalidateCache();

    // Draw the raster item
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

    // Get the pixel hint
    virtual QRectF pixelHint(const QRectF&) const;

    // Get the interval for a specific axis
    virtual QwtInterval interval(Qt::Axis) const;
    // Get the bounding rectangle
    virtual QRectF boundingRect() const override;

protected:
    // Render an image
    virtual QImage
    renderImage(const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& area, const QSize& imageSize) const = 0;

    /// Get the image map
    virtual QwtScaleMap
    imageMap(Qt::Orientation, const QwtScaleMap& map, const QRectF& area, const QSize& imageSize, double pixelSize) const;

private:
    explicit QwtPlotRasterItem(const QwtPlotRasterItem&);
    QwtPlotRasterItem& operator=(const QwtPlotRasterItem&);

    /// Initialize the raster item
    void init();

    /// Compose the image
    QImage compose(const QwtScaleMap&,
                   const QwtScaleMap&,
                   const QRectF& imageArea,
                   const QRectF& paintRect,
                   const QSize& imageSize,
                   bool doCache) const;

    QWT_DECLARE_PRIVATE(QwtPlotRasterItem)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotRasterItem::PaintAttributes)

#endif
