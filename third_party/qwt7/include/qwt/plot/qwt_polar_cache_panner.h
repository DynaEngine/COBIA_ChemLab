/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_PANNER_H
#define QWT_POLAR_PANNER_H

#include "qwt_global.h"
#include "qwt_cache_panner.h"
class QwtPolarPlot;
class QwtPolarCanvas;
/**
 * @brief QwtPolarCachePanner provides panning of a polar plot canvas
 * @details QwtPolarPanner is a panner for a QwtPolarCanvas, that
 *          adjusts the visible area after dropping
 *          the canvas on its new position.
 *
 *          Together with QwtPolarMagnifier individual ways
 *          of navigating on a QwtPolarPlot widget can be implemented easily.
 *
 * @sa QwtPolarMagnifier
 */
class QWT_EXPORT QwtPolarCachePanner : public QwtCachePanner
{
    Q_OBJECT

public:
    /// Constructor
    explicit QwtPolarCachePanner(QwtPolarCanvas*);
    /// Destructor
    ~QwtPolarCachePanner() override;

    /// Get the plot
    QwtPolarPlot* plot();
    /// Get the plot (const version)
    const QwtPolarPlot* plot() const;

    /// Get the canvas
    QwtPolarCanvas* canvas();
    /// Get the canvas (const version)
    const QwtPolarCanvas* canvas() const;

public Q_SLOTS:
    /// Move the plot
    virtual void movePlot(int dx, int dy);

protected:
    /// Handle mouse press events
    virtual void widgetMousePressEvent(QMouseEvent*) override;
};

#endif
