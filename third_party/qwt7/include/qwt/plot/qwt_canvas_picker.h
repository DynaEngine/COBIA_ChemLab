/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 2024   ChenZongYan <czy.t@163.com>
 *****************************************************************************/
#ifndef QWTCANVASPICKER_H
#define QWTCANVASPICKER_H
#include "qwt_picker.h"
// qt
class QWidget;
// qwt
class QwtPlot;

/**
 * @brief Base picker class specifically for canvas
 *
 * @details Provides convenient access methods for canvas and plot,
 * serving as a base class for other canvas-related tools.
 */
class QWT_EXPORT QwtCanvasPicker : public QwtPicker
{
    Q_OBJECT
public:
    // Constructor with canvas widget
    explicit QwtCanvasPicker(QWidget* canvas);
    // Destructor
    ~QwtCanvasPicker() override;

    // Get the associated plot (non-const version)
    QwtPlot* plot();
    // Get the associated plot (const version)
    const QwtPlot* plot() const;

    // Get the canvas widget (non-const version)
    QWidget* canvas();
    // Get the canvas widget (const version)
    const QWidget* canvas() const;
};

#endif  // QWTCANVASPICKER_H
