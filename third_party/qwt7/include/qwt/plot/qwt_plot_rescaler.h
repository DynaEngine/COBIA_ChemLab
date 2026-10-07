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

#ifndef QWT_PLOT_RESCALER_H
#define QWT_PLOT_RESCALER_H

#include "qwt_global.h"
#include "qwt_plot.h"

#include <qobject.h>

class QwtPlot;
class QwtInterval;
class QResizeEvent;

/**
 * @brief QwtPlotRescaler takes care of fixed aspect ratios for plot scales
 * @details QwtPlotRescaler auto adjusts the axes of a QwtPlot according
 *          to fixed aspect ratios.
 *
 */

class QWT_EXPORT QwtPlotRescaler : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Rescale policies
     * @details The rescale policy defines how to rescale the reference axis and
     *          their depending axes.
     * @sa ExpandingDirection, setIntervalHint()
     *
     */
    enum RescalePolicy
    {
        /**
         * The interval of the reference axis remains unchanged, when the
         * geometry of the canvas changes. All other axes
         * will be adjusted according to their aspect ratio.
         *
         */
        Fixed,

        /**
         * The interval of the reference axis will be shrunk/expanded,
         * when the geometry of the canvas changes. All other axes
         * will be adjusted according to their aspect ratio.
         *
         * The interval, that is represented by one pixel is fixed.
         *
         */
        Expanding,

        /**
         * The intervals of the axes are calculated, so that all axes include
         * their interval hint.
         *
         */
        Fitting
    };

    /**
     * @brief Expanding directions
     * @details When rescalePolicy() is set to Expanding its direction depends
     *          on ExpandingDirection
     *
     */
    enum ExpandingDirection
    {
        /// The upper limit of the scale is adjusted
        ExpandUp,

        /// The lower limit of the scale is adjusted
        ExpandDown,

        /// Both limits of the scale are adjusted
        ExpandBoth
    };

    // Constructs a rescaler for the given canvas with specified reference axis and policy
    explicit QwtPlotRescaler(QWidget* canvas, QwtAxisId referenceAxis = QwtAxis::XBottom, RescalePolicy = Expanding);

    // Destructs the rescaler
    ~QwtPlotRescaler() override;

    // Enable or disable the rescaler
    void setEnabled(bool);
    // Check if the rescaler is enabled
    bool isEnabled() const;

    // Set the rescale policy
    void setRescalePolicy(RescalePolicy);
    // Get the rescale policy
    RescalePolicy rescalePolicy() const;

    // Set the expanding direction for all axes
    void setExpandingDirection(ExpandingDirection);
    // Set the expanding direction for a specific axis
    void setExpandingDirection(QwtAxisId, ExpandingDirection);
    // Get the expanding direction for a specific axis
    ExpandingDirection expandingDirection(QwtAxisId) const;

    // Set the reference axis
    void setReferenceAxis(QwtAxisId);
    // Get the reference axis
    QwtAxisId referenceAxis() const;

    // Set the aspect ratio for all axes
    void setAspectRatio(double ratio);
    // Set the aspect ratio for a specific axis
    void setAspectRatio(QwtAxisId, double ratio);
    // Get the aspect ratio for a specific axis
    double aspectRatio(QwtAxisId) const;

    // Set the interval hint for a specific axis
    void setIntervalHint(QwtAxisId, const QwtInterval&);
    // Get the interval hint for a specific axis
    QwtInterval intervalHint(QwtAxisId) const;

    // Get the canvas widget
    QWidget* canvas();
    // Get the canvas widget (const version)
    const QWidget* canvas() const;

    // Get the plot widget
    QwtPlot* plot();
    // Get the plot widget (const version)
    const QwtPlot* plot() const;

    // Event filter for the plot canvas
    virtual bool eventFilter(QObject*, QEvent*) override;

    // Rescale the plot axes
    void rescale() const;

protected:
    // Handle canvas resize events
    virtual void canvasResizeEvent(QResizeEvent*);

    // Rescale the axes with old and new sizes
    virtual void rescale(const QSize& oldSize, const QSize& newSize) const;
    // Expand a scale interval
    virtual QwtInterval expandScale(QwtAxisId, const QSize& oldSize, const QSize& newSize) const;

    // Sync a scale to the reference axis
    virtual QwtInterval syncScale(QwtAxisId, const QwtInterval& reference, const QSize& size) const;

    // Update the axes scales
    virtual void updateScales(QwtInterval intervals[ QwtAxis::AxisPositions ]) const;

    // Get the orientation of an axis
    Qt::Orientation orientation(QwtAxisId) const;
    // Get the normalized interval of an axis
    QwtInterval interval(QwtAxisId) const;
    // Expand an interval by the specified width
    QwtInterval expandInterval(const QwtInterval&, double width, ExpandingDirection) const;

private:
    // Calculate the pixel distance for an axis
    double pixelDist(QwtAxisId, const QSize&) const;

    class AxisData;
    QWT_DECLARE_PRIVATE(QwtPlotRescaler)
};

#endif
