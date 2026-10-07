/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_PICKER_H
#define QWT_POLAR_PICKER_H

#include "qwt_global.h"
#include "qwt_picker.h"

#include <qvector.h>
#include <qpainterpath.h>

class QwtPolarPlot;
class QwtPolarCanvas;
class QwtPointPolar;

/**
 * @brief QwtPolarPicker provides selections on a plot canvas
 * @details QwtPolarPicker is a QwtPicker tailored for selections on
 *          a polar plot canvas.
 */
class QWT_EXPORT QwtPolarPicker : public QwtPicker
{
    Q_OBJECT

public:
    /// Constructor
    explicit QwtPolarPicker(QwtPolarCanvas*);
    /// Destructor
    ~QwtPolarPicker() override;

    /// Constructor with rubber band and tracker mode
    explicit QwtPolarPicker(RubberBand rubberBand, DisplayMode trackerMode, QwtPolarCanvas*);

    /// Get the plot
    QwtPolarPlot* plot();
    /// Get the plot (const version)
    const QwtPolarPlot* plot() const;

    /// Get the canvas
    QwtPolarCanvas* canvas();
    /// Get the canvas (const version)
    const QwtPolarCanvas* canvas() const;

    /// Get the pick rectangle
    virtual QRect pickRect() const;

Q_SIGNALS:

    /**
     * @brief A signal emitted in case of selectionFlags() & PointSelection
     * @param pos Selected point
     */
    void selected(const QwtPointPolar& pos);

    /**
     * @brief A signal emitting the selected points, at the end of a selection
     * @param points Selected points
     */
    void selected(const QVector< QwtPointPolar >& points);

    /**
     * @brief A signal emitted when a point has been appended to the selection
     * @param pos Position of the appended point
     * @sa append(), moved()
     */
    void appended(const QwtPointPolar& pos);

    /**
     * @brief A signal emitted whenever the last appended point of the selection has been moved
     * @param pos Position of the moved last point of the selection
     * @sa move(), appended()
     */
    void moved(const QwtPointPolar& pos);

protected:
    /// Inverse transform a point
    QwtPointPolar invTransform(const QPoint&) const;

    /// Get the tracker text
    virtual QwtText trackerText(const QPoint&) const override;
    /// Get the tracker text for a polar point
    virtual QwtText trackerTextPolar(const QwtPointPolar&) const;

    /// Move the selection
    virtual void move(const QPoint&) override;
    /// Append a point to the selection
    virtual void append(const QPoint&) override;
    /// End the selection
    virtual bool end(bool ok = true) override;

private:
    virtual QPainterPath pickArea() const override;

    QWT_DECLARE_PRIVATE(QwtPolarPicker)
};

#endif
