/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_LAYOUT_H
#define QWT_POLAR_LAYOUT_H

#include "qwt_global.h"
#include "qwt_polar_plot.h"

/**
 * @brief Layout class for QwtPolarPlot
 * @details Organizes the geometry for the different QwtPolarPlot components.
 *          It is used by the QwtPolar widget to organize its internal widgets
 *          or by QwtPolarRenderer to render its content to a QPaintDevice like
 *          a QPrinter, QPixmap/QImage or QSvgRenderer.
 */
class QWT_EXPORT QwtPolarLayout
{
public:
    //! @brief Options to configure the plot layout engine
    enum Option
    {
        //! Ignore the dimension of the scrollbars.
        IgnoreScrollbars = 0x01,

        //! Ignore all frames.
        IgnoreFrames = 0x02,

        //! Ignore the title.
        IgnoreTitle = 0x04,

        //! Ignore the legend.
        IgnoreLegend = 0x08
    };

    Q_DECLARE_FLAGS(Options, Option)

    /// Constructor
    explicit QwtPolarLayout();
    /// Destructor
    virtual ~QwtPolarLayout();

    /// Set the legend position with ratio
    void setLegendPosition(QwtPolarPlot::LegendPosition pos, double ratio);
    /// Set the legend position
    void setLegendPosition(QwtPolarPlot::LegendPosition pos);
    /// Get the legend position
    QwtPolarPlot::LegendPosition legendPosition() const;

    /// Set the legend ratio
    void setLegendRatio(double ratio);
    /// Get the legend ratio
    double legendRatio() const;

    /// Activate the layout
    virtual void activate(const QwtPolarPlot*, const QRectF& rect, Options options = Options());

    /// Invalidate the layout
    virtual void invalidate();

    /// Get the title rectangle
    const QRectF& titleRect() const;
    /// Get the legend rectangle
    const QRectF& legendRect() const;
    /// Get the canvas rectangle
    const QRectF& canvasRect() const;

    class LayoutData;

protected:
    QRectF layoutLegend(Options options, QRectF&) const;

private:
    QWT_DECLARE_PRIVATE(QwtPolarLayout)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPolarLayout::Options)

#endif
