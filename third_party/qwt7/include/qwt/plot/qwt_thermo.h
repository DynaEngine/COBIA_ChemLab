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

#ifndef QWT_THERMO_H
#define QWT_THERMO_H

#include "qwt_global.h"
#include "qwt_abstract_scale.h"
#include "qwt_interval.h"

class QwtScaleDraw;
class QwtColorMap;

/**
 * @brief The Thermometer Widget
 * @details QwtThermo is a widget which displays a value in an interval. It supports:
 *          - a horizontal or vertical layout;
 *          - a range;
 *          - a scale;
 *          - an alarm level.
 *          The fill colors might be calculated from an optional color map.
 *          If no color map has been assigned QwtThermo uses the following
 *          colors/brushes from the widget palette:
 *          - QPalette::Base: Background of the pipe
 *          - QPalette::ButtonText: Fill brush below the alarm level
 *          - QPalette::Highlight: Fill brush for the values above the alarm level
 *          - QPalette::WindowText: For the axis of the scale
 *          - QPalette::Text: For the labels of the scale
 * @sa QwtAbstractScale
 */
class QWT_EXPORT QwtThermo : public QwtAbstractScale
{
    Q_OBJECT

    Q_ENUMS(ScalePosition)
    Q_ENUMS(OriginMode)

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(ScalePosition scalePosition READ scalePosition WRITE setScalePosition)
    Q_PROPERTY(OriginMode originMode READ originMode WRITE setOriginMode)

    Q_PROPERTY(bool alarmEnabled READ alarmEnabled WRITE setAlarmEnabled)
    Q_PROPERTY(double alarmLevel READ alarmLevel WRITE setAlarmLevel)
    Q_PROPERTY(double origin READ origin WRITE setOrigin)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(int pipeWidth READ pipeWidth WRITE setPipeWidth)
    Q_PROPERTY(bool flatStyle READ flatStyle WRITE setFlatStyle)
    Q_PROPERTY(double value READ value WRITE setValue USER true)

public:
    /**
     * @brief Position of the scale
     * @sa setScalePosition(), setOrientation()
     */
    enum ScalePosition
    {
        /// The slider has no scale
        NoScale,

        /// The scale is right of a vertical or below of a horizontal slider
        LeadingScale,

        /// The scale is left of a vertical or above of a horizontal slider
        TrailingScale
    };

    /**
     * @brief Origin mode. This property specifies where the beginning of the liquid is placed.
     * @sa setOriginMode(), setOrigin()
     */
    enum OriginMode
    {
        /// The origin is the minimum of the scale
        OriginMinimum,

        /// The origin is the maximum of the scale
        OriginMaximum,

        /// The origin is specified using the origin() property
        OriginCustom
    };

    // Constructor
    explicit QwtThermo(QWidget* parent = nullptr);
    // Destructor
    ~QwtThermo() override;

    // Set the orientation
    void setOrientation(Qt::Orientation);
    // Return the orientation
    Qt::Orientation orientation() const;

    // Set the scale position
    void setScalePosition(ScalePosition);
    // Return the scale position
    ScalePosition scalePosition() const;

    // Set the spacing
    void setSpacing(int);
    // Return the spacing
    int spacing() const;

    // Set the border width
    void setBorderWidth(int);
    // Return the border width
    int borderWidth() const;

    // Set the origin mode
    void setOriginMode(OriginMode);
    // Return the origin mode
    OriginMode originMode() const;

    // Set the origin
    void setOrigin(double);
    // Return the origin
    double origin() const;

    // Set the fill brush
    void setFillBrush(const QBrush&);
    // Return the fill brush
    QBrush fillBrush() const;

    // Set the alarm brush
    void setAlarmBrush(const QBrush&);
    // Return the alarm brush
    QBrush alarmBrush() const;

    // Set the alarm level
    void setAlarmLevel(double);
    // Return the alarm level
    double alarmLevel() const;

    // Set whether alarm is enabled
    void setAlarmEnabled(bool);
    // Return whether alarm is enabled
    bool alarmEnabled() const;

    // Set the color map
    void setColorMap(QwtColorMap*);
    // Return the color map
    QwtColorMap* colorMap();
    // Return the color map (const version)
    const QwtColorMap* colorMap() const;

    // Set the pipe width
    void setPipeWidth(int);
    // Return the pipe width
    int pipeWidth() const;

    // Set flat style
    void setFlatStyle(bool);
    // Return flat style
    bool flatStyle() const;

    // Set the range flags
    void setRangeFlags(QwtInterval::BorderFlags);
    // Return the range flags
    QwtInterval::BorderFlags rangeFlags() const;

    // Return the current value
    double value() const;

    // Return the size hint
    virtual QSize sizeHint() const override;
    // Return the minimum size hint
    virtual QSize minimumSizeHint() const override;

    // Set the scale draw
    void setScaleDraw(QwtScaleDraw*);
    // Return the scale draw
    const QwtScaleDraw* scaleDraw() const;

public Q_SLOTS:
    // Set the current value
    virtual void setValue(double);

protected:
    /// Draw the liquid
    virtual void drawLiquid(QPainter*, const QRect&) const;
    /// Handle scale changes
    virtual void scaleChange() override;

    /// Handle paint events
    virtual void paintEvent(QPaintEvent*) override;
    /// Handle resize events
    virtual void resizeEvent(QResizeEvent*) override;
    /// Handle change events
    virtual void changeEvent(QEvent*) override;

    /// Return the scale draw
    QwtScaleDraw* scaleDraw();

    /// Return the pipe rectangle
    QRect pipeRect() const;
    /// Return the fill rectangle
    QRect fillRect(const QRect&) const;
    /// Return the alarm rectangle
    QRect alarmRect(const QRect&) const;

private:
    /// Layout the thermo
    void layoutThermo(bool);

    QWT_DECLARE_PRIVATE(QwtThermo)
};

#endif
