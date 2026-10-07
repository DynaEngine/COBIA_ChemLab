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

#ifndef QWT_DIAL_H
#define QWT_DIAL_H

#include "qwt_global.h"
#include "qwt_abstract_slider.h"

#include <qframe.h>
#include <qpalette.h>

class QwtDialNeedle;
class QwtRoundScaleDraw;
class QwtAbstractScaleDraw;

/**
 * @brief QwtDial class provides a rounded range control
 * @details QwtDial is intended as base class for dial widgets like
 *          speedometers, compass widgets, clocks...
 *          A dial contains a scale and a needle indicating the current value
 *          of the dial. Depending on Mode one of them is fixed and the
 *          other is rotating. If not isReadOnly() the
 *          dial can be rotated by dragging the mouse or using keyboard inputs
 *          (see QwtAbstractSlider::keyPressEvent()). A dial might be wrapping, what means
 *          a rotation below/above one limit continues on the other limit (f.e compass).
 *          The scale might cover any arc of the dial, its values are related to
 *          the origin() of the dial.
 *          Often dials have to be updated very often according to values from external
 *          devices. For these high refresh rates QwtDial caches as much as possible.
 *          For derived classes it might be necessary to clear these caches manually
 *          according to attribute changes using invalidateCache().
 * @sa QwtCompass, QwtAnalogClock, QwtDialNeedle
 * @note The controls and dials examples shows different types of dials.
 * @note QDial is more similar to QwtKnob than to QwtDial
 */

class QWT_EXPORT QwtDial : public QwtAbstractSlider
{
    Q_OBJECT

    Q_ENUMS(Shadow Mode Direction)

    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(Shadow frameShadow READ frameShadow WRITE setFrameShadow)
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(double origin READ origin WRITE setOrigin)
    Q_PROPERTY(double minScaleArc READ minScaleArc WRITE setMinScaleArc)
    Q_PROPERTY(double maxScaleArc READ maxScaleArc WRITE setMaxScaleArc)

public:
    /**
     * @brief Frame shadow
     * @details Unfortunately it is not possible to use QFrame::Shadow
     *          as a property of a widget that is not derived from QFrame.
     *          The following enum is made for the designer only. It is safe
     *          to use QFrame::Shadow instead.
     */
    enum Shadow
    {
        /// QFrame::Plain
        Plain = QFrame::Plain,

        /// QFrame::Raised
        Raised = QFrame::Raised,

        /// QFrame::Sunken
        Sunken = QFrame::Sunken
    };

    /**
     * @brief Mode controlling whether the needle or the scale is rotating
     */
    enum Mode
    {
        /// The needle is rotating
        RotateNeedle,

        /// The needle is fixed, the scales are rotating
        RotateScale
    };

    /// Constructor
    explicit QwtDial(QWidget* parent = nullptr);
    /// Destructor
    ~QwtDial() override;

    /// Set the frame shadow
    void setFrameShadow(Shadow);
    /// Return the frame shadow
    Shadow frameShadow() const;

    /// Set the line width
    void setLineWidth(int);
    /// Return the line width
    int lineWidth() const;

    /// Set the mode
    void setMode(Mode);
    /// Return the mode
    Mode mode() const;

    /// Set the scale arc range
    void setScaleArc(double minArc, double maxArc);

    /// Set the minimum scale arc
    void setMinScaleArc(double);
    /// Return the minimum scale arc
    double minScaleArc() const;

    /// Set the maximum scale arc
    void setMaxScaleArc(double);
    /// Return the maximum scale arc
    double maxScaleArc() const;

    /// Set the origin
    virtual void setOrigin(double);
    /// Return the origin
    double origin() const;

    /// Set the needle
    void setNeedle(QwtDialNeedle*);
    /// Return the needle (const version)
    const QwtDialNeedle* needle() const;
    /// Return the needle
    QwtDialNeedle* needle();

    /// Return the bounding rectangle
    QRect boundingRect() const;
    /// Return the inner rectangle
    QRect innerRect() const;

    /// Return the scale inner rectangle
    virtual QRect scaleInnerRect() const;

    /// Return the size hint
    virtual QSize sizeHint() const override;
    /// Return the minimum size hint
    virtual QSize minimumSizeHint() const override;

    /// Set the scale draw
    void setScaleDraw(QwtRoundScaleDraw*);

    /// Return the scale draw
    QwtRoundScaleDraw* scaleDraw();
    /// Return the scale draw (const version)
    const QwtRoundScaleDraw* scaleDraw() const;

protected:
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    virtual void changeEvent(QEvent*) override;

    virtual void drawFrame(QPainter*);
    virtual void drawContents(QPainter*) const;
    virtual void drawFocusIndicator(QPainter*) const;

    void invalidateCache();

    virtual void drawScale(QPainter*, const QPointF& center, double radius) const;

    virtual void drawScaleContents(QPainter* painter, const QPointF& center, double radius) const;

    virtual void drawNeedle(QPainter*, const QPointF&, double radius, double direction, QPalette::ColorGroup) const;

    virtual double scrolledTo(const QPoint&) const override;
    virtual bool isScrollPosition(const QPoint&) const override;

    virtual void sliderChange() override;
    virtual void scaleChange() override;

private:
    void setAngleRange(double angle, double span);
    void drawNeedle(QPainter*) const;

    QWT_DECLARE_PRIVATE(QwtDial)
};

#endif
