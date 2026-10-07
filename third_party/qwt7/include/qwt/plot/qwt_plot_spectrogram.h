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

#ifndef QWT_PLOT_SPECTROGRAM_H
#define QWT_PLOT_SPECTROGRAM_H

#include "qwt_global.h"
#include "qwt_raster_data.h"
#include "qwt_plot_rasteritem.h"

class QwtColorMap;
template< typename T >
class QList;

/**
 * @brief A plot item, which displays a spectrogram
 * @details A spectrogram displays 3-dimensional data, where the 3rd dimension
 *          ( the intensity ) is displayed using colors. The colors are calculated
 *          from the values using a color map.
 *
 *          On multi-core systems the performance of the image composition
 *          can often be improved by dividing the area into tiles - each of them
 *          rendered in a different thread ( see QwtPlotItem::setRenderThreadCount() ).
 *
 *          In ContourMode contour lines are painted for the contour levels.
 *
 * @sa QwtRasterData, QwtColorMap, QwtPlotItem::setRenderThreadCount()
 *
 */

class QWT_EXPORT QwtPlotSpectrogram : public QwtPlotRasterItem
{
public:
    /**
     * @brief Display modes
     * @details The display mode controls how the raster data will be represented.
     * @sa setDisplayMode(), testDisplayMode()
     *
     */

    enum DisplayMode
    {
        /// The values are mapped to colors using a color map.
        ImageMode = 0x01,

        /// The data is displayed using contour lines
        ContourMode = 0x02
    };

    Q_DECLARE_FLAGS(DisplayModes, DisplayMode)

    // Constructor
    explicit QwtPlotSpectrogram(const QString& title = QString());
    // Destructor
    ~QwtPlotSpectrogram() override;

    // Set a display mode
    void setDisplayMode(DisplayMode, bool on = true);
    // Test a display mode
    bool testDisplayMode(DisplayMode) const;

    // Set the raster data
    void setData(QwtRasterData* data);
    // Get the raster data (const)
    const QwtRasterData* data() const;
    // Get the raster data
    QwtRasterData* data();

    // Set the color map
    void setColorMap(QwtColorMap*);
    // Get the color map
    const QwtColorMap* colorMap() const;

    // Set the color table size
    void setColorTableSize(int numColors);
    // Get the color table size
    int colorTableSize() const;

    // Get the interval for an axis
    virtual QwtInterval interval(Qt::Axis) const override;
    // Get the pixel hint
    virtual QRectF pixelHint(const QRectF&) const override;

    // Set the default contour pen with color, width and style
    void setDefaultContourPen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    // Set the default contour pen
    void setDefaultContourPen(const QPen&);
    // Get the default contour pen
    QPen defaultContourPen() const;

    // Get the contour pen for a specific level
    virtual QPen contourPen(double level) const;

    // Set a conrec flag
    void setConrecFlag(QwtRasterData::ConrecFlag, bool on);
    // Test a conrec flag
    bool testConrecFlag(QwtRasterData::ConrecFlag) const;

    // Set the contour levels
    void setContourLevels(const QList< double >&);
    // Get the contour levels
    QList< double > contourLevels() const;

    // Get the runtime type information
    virtual int rtti() const override;

    // Draw the spectrogram
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const override;

protected:
    /**
     * @brief Render the image
     */
    virtual QImage
    renderImage(const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& area, const QSize& imageSize) const override;

    /**
     * @brief Calculate the contour raster size
     */
    virtual QSize contourRasterSize(const QRectF&, const QRect&) const;

    /**
     * @brief Render the contour lines
     */
    virtual QwtRasterData::ContourLines renderContourLines(const QRectF& rect, const QSize& raster) const;

    /**
     * @brief Draw the contour lines
     */
    virtual void
    drawContourLines(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QwtRasterData::ContourLines&) const;

    /**
     * @brief Render a tile
     */
    void renderTile(const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRect& tile, QImage*) const;

private:
    QWT_DECLARE_PRIVATE(QwtPlotSpectrogram)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotSpectrogram::DisplayModes)

#endif
