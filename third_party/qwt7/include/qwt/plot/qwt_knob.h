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

#ifndef QWT_KNOB_H
#define QWT_KNOB_H

#include "qwt_global.h"
#include "qwt_abstract_slider.h"

class QwtRoundScaleDraw;

/**
 * @brief The Knob Widget
 * @details The QwtKnob widget imitates look and behavior of a volume knob on a radio.
 *          It looks similar to QDial - not to QwtDial.
 *          The value range of a knob might be divided into several turns.
 *          The layout of the knob depends on the knobWidth():
 *          - width > 0: The diameter of the knob is fixed and the knob is aligned
 *            according to the alignment() flags inside of the contentsRect().
 *          - width <= 0: The knob is extended to the minimum of width/height of
 *            the contentsRect() and aligned in the other direction according to alignment().
 *          Setting a fixed knobWidth() is helpful to align several knobs with different scale labels.
 * @image html knob.png
 */

class QWT_EXPORT QwtKnob : public QwtAbstractSlider
{
    Q_OBJECT

    Q_ENUMS(KnobStyle MarkerStyle)

    Q_PROPERTY(KnobStyle knobStyle READ knobStyle WRITE setKnobStyle)
    Q_PROPERTY(int knobWidth READ knobWidth WRITE setKnobWidth)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(double totalAngle READ totalAngle WRITE setTotalAngle)
    Q_PROPERTY(int numTurns READ numTurns WRITE setNumTurns)
    Q_PROPERTY(MarkerStyle markerStyle READ markerStyle WRITE setMarkerStyle)
    Q_PROPERTY(int markerSize READ markerSize WRITE setMarkerSize)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)

public:
    /**
     * @brief Style of the knob surface
     * @details Depending on the KnobStyle the surface of the knob is
     *          filled from the brushes of the widget palette().
     * @sa setKnobStyle(), knobStyle()
     */
    enum KnobStyle
    {
        /// Fill the knob with a brush from QPalette::Button
        Flat,

        /// Build a gradient from QPalette::Midlight and QPalette::Button
        Raised,

        /// Build a gradient from QPalette::Midlight, QPalette::Button and QPalette::Midlight
        Sunken,

        /// Build a radial gradient from QPalette::Button like QDial in various Qt styles
        Styled
    };

    /**
     * @brief Marker type
     * @details The marker indicates the current value on the knob.
     *          The default setting is a Notch marker.
     * @sa setMarkerStyle(), setMarkerSize()
     */
    enum MarkerStyle
    {
        /// Don't paint any marker
        NoMarker = -1,

        /// Paint a single tick in QPalette::ButtonText color
        Tick,

        /// Paint a triangle in QPalette::ButtonText color
        Triangle,

        /// Paint a circle in QPalette::ButtonText color
        Dot,

        /// Draw a raised ellipse with a gradient from QPalette::Light and QPalette::Mid
        Nub,

        /// Draw a sunken ellipse with a gradient from QPalette::Light and QPalette::Mid
        Notch
    };

    /// Constructor
    explicit QwtKnob(QWidget* parent = nullptr);
    /// Destructor
    ~QwtKnob() override;

    /// Set alignment of the knob inside contentsRect()
    void setAlignment(Qt::Alignment);
    /// Return alignment of the knob
    Qt::Alignment alignment() const;

    /// Set the knob's width (diameter)
    void setKnobWidth(int);
    /// Return the knob's width
    int knobWidth() const;

    /// Set the number of turns for the knob
    void setNumTurns(int);
    /// Return the number of turns
    int numTurns() const;

    /// Set the total angle which the knob can be turned
    void setTotalAngle(double angle);
    /// Return the total angle
    double totalAngle() const;

    /// Set the knob style
    void setKnobStyle(KnobStyle);
    /// Return the knob style
    KnobStyle knobStyle() const;

    /// Set the border width
    void setBorderWidth(int);
    /// Return the border width
    int borderWidth() const;

    /// Set the marker style
    void setMarkerStyle(MarkerStyle);
    /// Return the marker style
    MarkerStyle markerStyle() const;

    /// Set the marker size
    void setMarkerSize(int);
    /// Return the marker size
    int markerSize() const;

    /// Return size hint
    virtual QSize sizeHint() const override;
    /// Return minimum size hint
    virtual QSize minimumSizeHint() const override;

    /// Set the scale draw
    void setScaleDraw(QwtRoundScaleDraw*);

    /// Return the scale draw (const version)
    const QwtRoundScaleDraw* scaleDraw() const;
    /// Return the scale draw (non-const version)
    QwtRoundScaleDraw* scaleDraw();

    /// Return the bounding rectangle of the knob
    QRect knobRect() const;

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void changeEvent(QEvent*) override;

    virtual void drawKnob(QPainter*, const QRectF&) const;

    virtual void drawFocusIndicator(QPainter*) const;

    virtual void drawMarker(QPainter*, const QRectF&, double angle) const;

    virtual double scrolledTo(const QPoint&) const override;
    virtual bool isScrollPosition(const QPoint&) const override;

private:
    QWT_DECLARE_PRIVATE(QwtKnob)
};

#endif
