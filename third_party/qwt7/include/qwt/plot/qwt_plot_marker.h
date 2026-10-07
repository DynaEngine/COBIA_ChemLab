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

#ifndef QWT_PLOT_MARKER_H
#define QWT_PLOT_MARKER_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

class QString;
class QRectF;
class QwtText;
class QwtSymbol;

/**
 * @brief A class for drawing markers
 * @details A marker can be a horizontal line, a vertical line,
 *          a symbol, a label or any combination of them, which can
 *          be drawn around a center point inside a bounding rectangle.
 *
 *          The setSymbol() member assigns a symbol to the marker.
 *          The symbol is drawn at the specified point.
 *
 *          With setLabel(), a label can be assigned to the marker.
 *          The setLabelAlignment() member specifies where the label is
 *          drawn. All the Align*-constants in Qt::AlignmentFlags (see Qt documentation)
 *          are valid. The interpretation of the alignment depends on the marker's
 *          line style. The alignment refers to the center point of
 *          the marker, which means, for example, that the label would be printed
 *          left above the center point if the alignment was set to
 *          Qt::AlignLeft | Qt::AlignTop.
 *
 * @note QwtPlotTextLabel is intended to align a text label
 *       according to the geometry of canvas
 *       ( unrelated to plot coordinates )
 *
 */

class QWT_EXPORT QwtPlotMarker : public QwtPlotItem
{
public:
    /**
     * @brief Line styles
     * @sa setLineStyle(), lineStyle()
     *
     */
    enum LineStyle
    {
        /// No line
        NoLine,

        /// A horizontal line
        HLine,

        /// A vertical line
        VLine,

        /// A crosshair
        Cross
    };

    /// Constructor
    explicit QwtPlotMarker();
    /// Constructor with title
    explicit QwtPlotMarker(const QString& title);
    /// Constructor with QwtText title
    explicit QwtPlotMarker(const QwtText& title);

    /// Destructor
    ~QwtPlotMarker() override;

    /// Get the runtime type information
    virtual int rtti() const override;

    /// Get the x-value
    double xValue() const;
    /// Get the y-value
    double yValue() const;
    /// Get the value as a point
    QPointF value() const;

    /// Set the x-value
    void setXValue(double);
    /// Set the y-value
    void setYValue(double);
    /// Set the value
    void setValue(double, double);
    /// Set the value from a point
    void setValue(const QPointF&);

    /// Set the line style
    void setLineStyle(LineStyle);
    /// Get the line style
    LineStyle lineStyle() const;

    /// Set the line pen
    void setLinePen(const QColor&, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    /// Set the line pen
    void setLinePen(const QPen&);
    /// Get the line pen
    const QPen& linePen() const;

    /// Set the symbol
    void setSymbol(const QwtSymbol*);
    /// Get the symbol
    const QwtSymbol* symbol() const;

    /// Set the label
    void setLabel(const QwtText&);
    /// Get the label
    QwtText label() const;

    /// Set the label alignment
    void setLabelAlignment(Qt::Alignment);
    /// Get the label alignment
    Qt::Alignment labelAlignment() const;

    /// Set the label orientation
    void setLabelOrientation(Qt::Orientation);
    /// Get the label orientation
    Qt::Orientation labelOrientation() const;

    /// Set the spacing
    void setSpacing(int);
    /// Get the spacing
    int spacing() const;

    /// Draw the marker
    virtual void draw(QPainter*, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF&) const override;

    /// Get the bounding rectangle
    virtual QRectF boundingRect() const override;

    /// Get the legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const override;

protected:
    /// Draw the lines
    virtual void drawLines(QPainter*, const QRectF&, const QPointF&) const;

    /// Draw the symbol
    virtual void drawSymbol(QPainter*, const QRectF&, const QPointF&) const;

    /// Draw the label
    virtual void drawLabel(QPainter*, const QRectF&, const QPointF&) const;

private:
    QWT_DECLARE_PRIVATE(QwtPlotMarker)
};

#endif
