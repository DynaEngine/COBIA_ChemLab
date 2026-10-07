/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_MAGNIFIER_H
#define QWT_POLAR_MAGNIFIER_H

#include "qwt_global.h"
#include "qwt_magnifier.h"

class QwtPolarPlot;
class QwtPolarCanvas;

/**
 * @brief QwtPolarMagnifier provides zooming, by magnifying in steps
 * @details Using QwtPlotMagnifier a plot can be zoomed in/out in steps using
 *          keys, the mouse wheel or moving a mouse button in vertical direction.
 *
 *          Together with QwtPolarPanner it is possible to implement
 *          an individual navigation of the plot canvas.
 *
 * @sa QwtPolarPanner, QwtPolarPlot, QwtPolarCanvas
 */
class QWT_EXPORT QwtPolarMagnifier : public QwtMagnifier
{
    Q_OBJECT

public:
    /// Constructor
    explicit QwtPolarMagnifier(QwtPolarCanvas*);
    /// Destructor
    ~QwtPolarMagnifier() override;

    /// Set the unzoom key
    void setUnzoomKey(int key, int modifiers);
    /// Get the unzoom key
    void getUnzoomKey(int& key, int& modifiers) const;

    /// Get the plot
    QwtPolarPlot* plot();
    /// Get the plot (const version)
    const QwtPolarPlot* plot() const;

    /// Get the canvas
    QwtPolarCanvas* canvas();
    /// Get the canvas (const version)
    const QwtPolarCanvas* canvas() const;

public Q_SLOTS:
    /// Rescale the plot
    virtual void rescale(double factor) override;
    /// Unzoom the plot
    void unzoom();

protected:
    /// Handle key press events
    virtual void widgetKeyPressEvent(QKeyEvent*) override;

private:
    QWT_DECLARE_PRIVATE(QwtPolarMagnifier)
};

#endif
