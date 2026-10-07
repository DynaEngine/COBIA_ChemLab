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

#ifndef QWT_NULL_PAINT_DEVICE_H
#define QWT_NULL_PAINT_DEVICE_H

#include "qwt_global.h"

#include <qpaintdevice.h>
#include <qpaintengine.h>

/**
 * @brief A null paint device that does nothing
 * @details Sometimes important layout/rendering geometries are not
 *          available or changeable from the public Qt class interface
 *          (e.g., hidden in the style implementation).
 *          QwtNullPaintDevice can be used to manipulate or filter out
 *          this information by analyzing the stream of paint primitives.
 *          For example, QwtNullPaintDevice is used by QwtPlotCanvas to identify
 *          styled backgrounds with rounded corners.
 */
class QWT_EXPORT QwtNullPaintDevice : public QPaintDevice
{
public:
    /**
     * @brief Render mode for the paint device
     * @details Controls how vector graphic primitives are processed.
     */
    enum Mode
    {
        //! All vector graphic primitives are painted by corresponding draw methods
        NormalMode,

        //! Vector graphic primitives (beside polygons) are mapped to QPainterPath
        PolygonPathMode,

        //! All vector graphic primitives are mapped to QPainterPath
        PathMode
    };

    //! Constructor
    QwtNullPaintDevice();
    //! Destructor
    ~QwtNullPaintDevice() override;

    //! Set the render mode
    void setMode(Mode);
    //! Get the render mode
    Mode mode() const;

    //! Return the paint engine
    virtual QPaintEngine* paintEngine() const override;

    //! Return metric information for the paint device
    virtual int metric(PaintDeviceMetric) const override;

    //! Draw rectangles (integer version)
    virtual void drawRects(const QRect*, int);
    //! Draw rectangles (floating point version)
    virtual void drawRects(const QRectF*, int);

    //! Draw lines (integer version)
    virtual void drawLines(const QLine*, int);
    //! Draw lines (floating point version)
    virtual void drawLines(const QLineF*, int);

    //! Draw ellipse (floating point version)
    virtual void drawEllipse(const QRectF&);
    //! Draw ellipse (integer version)
    virtual void drawEllipse(const QRect&);

    //! Draw a painter path
    virtual void drawPath(const QPainterPath&);

    //! Draw points (floating point version)
    virtual void drawPoints(const QPointF*, int);
    //! Draw points (integer version)
    virtual void drawPoints(const QPoint*, int);

    //! Draw polygon (floating point version)
    virtual void drawPolygon(const QPointF*, int, QPaintEngine::PolygonDrawMode);

    //! Draw polygon (integer version)
    virtual void drawPolygon(const QPoint*, int, QPaintEngine::PolygonDrawMode);

    //! Draw a pixmap
    virtual void drawPixmap(const QRectF&, const QPixmap&, const QRectF&);

    //! Draw a text item
    virtual void drawTextItem(const QPointF&, const QTextItem&);

    //! Draw a tiled pixmap
    virtual void drawTiledPixmap(const QRectF&, const QPixmap&, const QPointF&);

    //! Draw an image
    virtual void drawImage(const QRectF&, const QImage&, const QRectF&, Qt::ImageConversionFlags);

    //! Update the paint engine state
    virtual void updateState(const QPaintEngineState&);

protected:
    //! @return Size needed to implement metric()
    virtual QSize sizeMetrics() const = 0;

private:
    class PaintEngine;
    PaintEngine* m_engine;

    QWT_DECLARE_PRIVATE(QwtNullPaintDevice)
};

#endif
