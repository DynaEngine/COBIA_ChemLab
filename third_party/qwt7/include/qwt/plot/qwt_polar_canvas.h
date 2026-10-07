/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_CANVAS_H
#define QWT_POLAR_CANVAS_H

#include "qwt_global.h"
#include "qwt_point_polar.h"
#include <qframe.h>
class QPainter;
class QwtPolarPlot;

/**
 * @brief Canvas of a QwtPolarPlot
 * @details The canvas is the widget, where all polar items are painted to.
 *
 * @note In opposite to QwtPlot all axes are painted on the canvas.
 * @sa QwtPolarPlot
 */
class QWT_EXPORT QwtPolarCanvas : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief Paint attributes
     * @details The default setting enables BackingStore
     * @sa setPaintAttribute(), testPaintAttribute(), backingStore()
     */
    enum PaintAttribute
    {
        /**
         * Paint double buffered and reuse the content of the pixmap buffer
         * for some spontaneous repaints that happen when a plot gets unhidden,
         * deiconified or changes the focus.
         */
        BackingStore = 0x01
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    /// Constructor
    explicit QwtPolarCanvas(QwtPolarPlot*);
    /// Destructor
    ~QwtPolarCanvas() override;

    /// Get the plot
    QwtPolarPlot* plot();
    /// Get the plot (const version)
    const QwtPolarPlot* plot() const;

    /// Set a paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    /// Test a paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    /// Get the backing store
    const QPixmap* backingStore() const;
    /// Invalidate the backing store
    void invalidateBackingStore();

    /// Inverse transform from widget coordinates to polar coordinates
    QwtPointPolar invTransform(const QPoint&) const;
    /// Transform from polar coordinates to widget coordinates
    QPoint transform(const QwtPointPolar&) const;

protected:
    /// Handle paint events
    virtual void paintEvent(QPaintEvent*) override;
    /// Handle resize events
    virtual void resizeEvent(QResizeEvent*) override;

private:
    QWT_DECLARE_PRIVATE(QwtPolarCanvas)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPolarCanvas::PaintAttributes)

#endif
