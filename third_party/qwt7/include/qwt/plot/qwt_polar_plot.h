/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_PLOT_H
#define QWT_POLAR_PLOT_H

#include "qwt_global.h"
#include "qwt_polar.h"
#include "qwt_polar_itemdict.h"
#include "qwt_interval.h"
#include "qwt_scale_map.h"
#include "qwt_point_polar.h"
#include <qframe.h>

class QwtRoundScaleDraw;
class QwtScaleEngine;
class QwtScaleDiv;
class QwtTextLabel;
class QwtPolarCanvas;
class QwtPolarLayout;
class QwtAbstractLegend;

/**
 * @brief A plotting widget, displaying a polar coordinate system
 * @details An unlimited number of plot items can be displayed on
 *          its canvas. Plot items might be curves (QwtPolarCurve), markers
 *          (QwtPolarMarker), the grid (QwtPolarGrid), or anything else derived
 *          from QwtPolarItem.
 *
 *          The coordinate system is defined by a radial and a azimuth scale.
 *          The scales at the axes can be explicitly set (QwtScaleDiv), or
 *          are calculated from the plot items, using algorithms (QwtScaleEngine) which
 *          can be configured separately for each axis. Autoscaling is supported
 *          for the radial scale.
 *
 *          In opposite to QwtPlot the scales might be different from the
 *          view, that is displayed on the canvas. The view can be changed by
 *          zooming - f.e. by using QwtPolarPanner or QwtPolarMaginfier.
 */
class QWT_EXPORT QwtPolarPlot : public QFrame, public QwtPolarItemDict
{
    Q_OBJECT

    Q_PROPERTY(QBrush plotBackground READ plotBackground WRITE setPlotBackground)
    Q_PROPERTY(double azimuthOrigin READ azimuthOrigin WRITE setAzimuthOrigin)

public:
    /**
     * @brief Position of the legend, relative to the canvas
     * @sa insertLegend()
     */
    enum LegendPosition
    {
        /// The legend will be left from the canvas
        LeftLegend,

        /// The legend will be right from the canvas
        RightLegend,

        /// The legend will be below the canvas
        BottomLegend,

        /// The legend will be between canvas and title
        TopLegend,

        /**
         * External means that only the content of the legend
         * will be handled by QwtPlot, but not its geometry.
         * This might be interesting if an application wants to
         * have a legend in an external window ( or on the canvas ).
         *
         * @note The legend is not painted by QwtPolarRenderer
         */
        ExternalLegend
    };

    /// Constructor
    explicit QwtPolarPlot(QWidget* parent = nullptr);
    /// Constructor with title
    QwtPolarPlot(const QwtText& title, QWidget* parent = nullptr);

    /// Destructor
    ~QwtPolarPlot() override;

    /// Set the title
    void setTitle(const QString&);
    /// Set the title
    void setTitle(const QwtText&);

    /// Get the title
    QwtText title() const;

    /// Get the title label
    QwtTextLabel* titleLabel();
    /// Get the title label (const version)
    const QwtTextLabel* titleLabel() const;

    /// Set auto replot
    void setAutoReplot(bool tf = true);
    /// Get auto replot
    bool autoReplot() const;

    /// Set auto scale for a scale
    void setAutoScale(int scaleId);
    /// Check if a scale has auto scale
    bool hasAutoScale(int scaleId) const;

    /// Set the maximum number of minor ticks for a scale
    void setScaleMaxMinor(int scaleId, int maxMinor);
    /// Get the maximum number of minor ticks for a scale
    int scaleMaxMinor(int scaleId) const;

    /// Get the maximum number of major ticks for a scale
    int scaleMaxMajor(int scaleId) const;
    /// Set the maximum number of major ticks for a scale
    void setScaleMaxMajor(int scaleId, int maxMajor);

    /// Get the scale engine for a scale
    QwtScaleEngine* scaleEngine(int scaleId);
    /// Get the scale engine for a scale (const version)
    const QwtScaleEngine* scaleEngine(int scaleId) const;
    /// Set the scale engine for a scale
    void setScaleEngine(int scaleId, QwtScaleEngine*);

    /// Set the scale for a scale
    void setScale(int scaleId, double min, double max, double step = 0);

    /// Set the scale division for a scale
    void setScaleDiv(int scaleId, const QwtScaleDiv&);
    /// Get the scale division for a scale (const version)
    const QwtScaleDiv* scaleDiv(int scaleId) const;
    /// Get the scale division for a scale
    QwtScaleDiv* scaleDiv(int scaleId);

    /// Get the scale map for a scale with radius
    QwtScaleMap scaleMap(int scaleId, double radius) const;
    /// Get the scale map for a scale
    QwtScaleMap scaleMap(int scaleId) const;

    /// Update a scale
    void updateScale(int scaleId);

    /// Get the azimuth origin
    double azimuthOrigin() const;

    /// Zoom to a position with a factor
    void zoom(const QwtPointPolar&, double factor);
    /// Unzoom the plot
    void unzoom();

    /// Get the zoom position
    QwtPointPolar zoomPos() const;
    /// Get the zoom factor
    double zoomFactor() const;

    // Canvas

    /// Get the canvas
    QwtPolarCanvas* canvas();
    /// Get the canvas (const version)
    const QwtPolarCanvas* canvas() const;

    /// Set the plot background
    void setPlotBackground(const QBrush& c);
    /// Get the plot background
    const QBrush& plotBackground() const;

    /// Set the background color
    void setBackgroundColor(const QColor& c);
    /// Get the background color
    QColor backgroundColor() const;

    /// Draw the canvas
    virtual void drawCanvas(QPainter*, const QRectF&) const;

    // Legend

    /// Insert a legend
    void insertLegend(QwtAbstractLegend*, LegendPosition = RightLegend, double ratio = -1.0);

    /// Get the legend
    QwtAbstractLegend* legend();
    /// Get the legend (const version)
    const QwtAbstractLegend* legend() const;

    /// Update the legend
    void updateLegend();
    /// Update the legend for an item
    void updateLegend(const QwtPolarItem*);

    // Layout
    /// Get the plot layout
    QwtPolarLayout* plotLayout();
    /// Get the plot layout (const version)
    const QwtPolarLayout* plotLayout() const;

    /// Get the visible interval
    QwtInterval visibleInterval() const;
    /// Get the plot rectangle
    QRectF plotRect() const;
    /// Get the plot rectangle for a given rectangle
    QRectF plotRect(const QRectF&) const;

    /// Get the plot margin hint
    int plotMarginHint() const;

    /// Convert an item to info
    virtual QVariant itemToInfo(QwtPolarItem*) const;
    /// Convert info to an item
    virtual QwtPolarItem* infoToItem(const QVariant&) const;

Q_SIGNALS:
    /**
     * @brief A signal indicating, that an item has been attached/detached
     * @param plotItem Plot item
     * @param on Attached/Detached
     */
    void itemAttached(QwtPolarItem* plotItem, bool on);

    /**
     * @brief A signal with the attributes how to update the legend entries for a plot item
     * @param itemInfo Info about a plot, build from itemToInfo()
     * @param data Attributes of the entries ( usually <= 1 ) for the plot item
     * @sa itemToInfo(), infoToItem(), QwtAbstractLegend::updateLegend()
     */
    void legendDataChanged(const QVariant& itemInfo, const QList< QwtLegendData >& data);

    /**
     * @brief A signal that is emitted, whenever the layout of the plot has been recalculated
     */
    void layoutChanged();

public Q_SLOTS:
    /// Replot the plot
    virtual void replot();
    /// Auto refresh the plot
    void autoRefresh();
    /// Set the azimuth origin
    void setAzimuthOrigin(double);

protected:
    /// Handle events
    virtual bool event(QEvent*) override;
    /// Handle resize events
    virtual void resizeEvent(QResizeEvent*) override;

    /// Update the layout
    virtual void updateLayout();

    /// Draw items
    virtual void drawItems(QPainter* painter,
                           const QwtScaleMap& radialMap,
                           const QwtScaleMap& azimuthMap,
                           const QPointF& pole,
                           double radius,
                           const QRectF& canvasRect) const;

private:
    friend class QwtPolarItem;
    void attachItem(QwtPolarItem*, bool);

    void initPlot(const QwtText&);

    QWT_DECLARE_PRIVATE(QwtPolarPlot)
};

#endif
