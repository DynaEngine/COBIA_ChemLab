/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_RENDERER_H
#define QWT_POLAR_RENDERER_H

#include "qwt_global.h"
#include <qobject.h>
#include <qsize.h>

class QwtPolarPlot;
class QRectF;
class QPainter;
class QPaintDevice;

#ifndef QT_NO_PRINTER
class QPrinter;
#endif

#ifndef QWT_NO_SVG
#ifdef QT_SVG_LIB
class QSvgGenerator;
#endif
#endif

/**
 * @brief Renderer for exporting a polar plot to a document, a printer
 *        or anything else, that is supported by QPainter/QPaintDevice
 */
class QWT_EXPORT QwtPolarRenderer : public QObject
{
    Q_OBJECT

public:
    /// Constructor
    explicit QwtPolarRenderer(QObject* parent = nullptr);
    /// Destructor
    ~QwtPolarRenderer() override;

    /// Render a polar plot to a document
    void renderDocument(QwtPolarPlot*, const QString& format, const QSizeF& sizeMM, int resolution = 85);

    /// Render a polar plot to a document with title
    void renderDocument(QwtPolarPlot*, const QString& title, const QString& format, const QSizeF& sizeMM, int resolution = 85);

#ifndef QWT_NO_SVG
#ifdef QT_SVG_LIB
    /// Render a polar plot to a SVG generator
    void renderTo(QwtPolarPlot*, QSvgGenerator&) const;
#endif
#endif

#ifndef QT_NO_PRINTER
    /// Render a polar plot to a printer
    void renderTo(QwtPolarPlot*, QPrinter&) const;
#endif

    /// Render a polar plot to a paint device
    void renderTo(QwtPolarPlot*, QPaintDevice&) const;

    /// Render a polar plot to a painter
    virtual void render(QwtPolarPlot*, QPainter*, const QRectF& rect) const;

    /// Export a polar plot to a document
    bool exportTo(QwtPolarPlot*, const QString& documentName, const QSizeF& sizeMM = QSizeF(200, 200), int resolution = 85);

    /// Render the title
    virtual void renderTitle(QPainter*, const QRectF&) const;

    /// Render the legend
    virtual void renderLegend(const QwtPolarPlot*, QPainter*, const QRectF&) const;

private:
    QWT_DECLARE_PRIVATE(QwtPolarRenderer)
};

#endif
