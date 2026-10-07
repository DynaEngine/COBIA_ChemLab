/******************************************************************************
 * QwtPolar Widget Library
 * Copyright (C) 2008   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_POLAR_GRID_H
#define QWT_POLAR_GRID_H

#include "qwt_global.h"
#include "qwt_polar.h"
#include "qwt_polar_item.h"
#include "qwt_polar_plot.h"

class QPainter;
class QPen;
class QwtScaleMap;
class QwtScaleDiv;
class QwtRoundScaleDraw;
class QwtScaleDraw;

/**
 * @brief An item which draws scales and grid lines on a polar plot
 * @details The QwtPolarGrid class can be used to draw a coordinate grid.
 *          A coordinate grid consists of major and minor gridlines.
 *          The locations of the gridlines are determined by the azimuth and radial
 *          scale divisions.
 *
 *          QwtPolarGrid is also responsible for drawing the axis representing the
 *          scales. It is possible to display 4 radial and one azimuth axis.
 *
 *          Whenever the scale divisions of the plot widget changes the grid
 *          is synchronized by updateScaleDiv().
 *
 * @sa QwtPolarPlot, QwtPolar::Axis
 */
class QWT_EXPORT QwtPolarGrid : public QwtPolarItem
{
public:
    /**
     * @brief Display flags to avoid conflicts when painting scales and grid lines
     * @details The default setting enables all flags.
     * @sa setDisplayFlag(), testDisplayFlag()
     */
    enum DisplayFlag
    {
        /**
         * Try to avoid situations, where the label of the origin is
         * painted over another axis.
         */
        SmartOriginLabel = 1,

        /**
         * Often the outermost tick of the radial scale is close to the
         * canvas border. With HideMaxRadiusLabel enabled it is not painted.
         */
        HideMaxRadiusLabel = 2,

        /**
         * The tick labels of the radial scales might be hard to read, when
         * they are painted on top of the radial grid lines ( or on top
         * of a curve/spectrogram ). When ClipAxisBackground the bounding rect
         * of each label is added to the clip region.
         */
        ClipAxisBackground = 4,

        /**
         * Don't paint the backbone of the radial axes, when they are very close
         * to a line of the azimuth grid.
         */
        SmartScaleDraw = 8,

        /**
         * All grid lines are clipped against the plot area before being painted.
         * When the plot is zoomed in this will have an significant impact
         * on the performance of the painting code.
         */
        ClipGridLines = 16
    };

    Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag)

    /**
     * @brief Grid attributes
     * @sa setGridAttributes(), testGridAttributes()
     */
    enum GridAttribute
    {
        /**
         * When AutoScaling is enabled, the radial axes will be adjusted
         * to the interval, that is currently visible on the canvas plot.
         */
        AutoScaling = 0x01
    };

    Q_DECLARE_FLAGS(GridAttributes, GridAttribute)

    /// Constructor
    explicit QwtPolarGrid();
    /// Destructor
    ~QwtPolarGrid() override;

    /// Get the runtime type information
    virtual int rtti() const override;

    /// Set a display flag
    void setDisplayFlag(DisplayFlag, bool on = true);
    /// Test a display flag
    bool testDisplayFlag(DisplayFlag) const;

    /// Set a grid attribute
    void setGridAttribute(GridAttribute, bool on = true);
    /// Test a grid attribute
    bool testGridAttribute(GridAttribute) const;

    /// Show/hide the grid for a scale
    void showGrid(int scaleId, bool show = true);
    /// Check if the grid is visible for a scale
    bool isGridVisible(int scaleId) const;

    /// Show/hide the minor grid for a scale
    void showMinorGrid(int scaleId, bool show = true);
    /// Check if the minor grid is visible for a scale
    bool isMinorGridVisible(int scaleId) const;

    /// Show/hide an axis
    void showAxis(int axisId, bool show = true);
    /// Check if an axis is visible
    bool isAxisVisible(int axisId) const;

    /// Set the pen
    void setPen(const QPen& p);
    /// Set the font
    void setFont(const QFont&);

    /// Set the major grid pen
    void setMajorGridPen(const QPen& p);
    /// Set the major grid pen for a scale
    void setMajorGridPen(int scaleId, const QPen& p);
    /// Get the major grid pen for a scale
    QPen majorGridPen(int scaleId) const;

    /// Set the minor grid pen
    void setMinorGridPen(const QPen& p);
    /// Set the minor grid pen for a scale
    void setMinorGridPen(int scaleId, const QPen& p);
    /// Get the minor grid pen for a scale
    QPen minorGridPen(int scaleId) const;

    /// Set the axis pen
    void setAxisPen(int axisId, const QPen& p);
    /// Get the axis pen
    QPen axisPen(int axisId) const;

    /// Set the axis font
    void setAxisFont(int axisId, const QFont& p);
    /// Get the axis font
    QFont axisFont(int axisId) const;

    /// Set the scale draw for an axis
    void setScaleDraw(int axisId, QwtScaleDraw*);
    /// Get the scale draw for an axis (const version)
    const QwtScaleDraw* scaleDraw(int axisId) const;
    /// Get the scale draw for an axis
    QwtScaleDraw* scaleDraw(int axisId);

    /// Set the azimuth scale draw
    void setAzimuthScaleDraw(QwtRoundScaleDraw*);
    /// Get the azimuth scale draw (const version)
    const QwtRoundScaleDraw* azimuthScaleDraw() const;
    /// Get the azimuth scale draw
    QwtRoundScaleDraw* azimuthScaleDraw();

    /// Draw the grid
    virtual void draw(QPainter* p,
                      const QwtScaleMap& azimuthMap,
                      const QwtScaleMap& radialMap,
                      const QPointF& pole,
                      double radius,
                      const QRectF& rect) const override;

    /// Update the scale division
    virtual void updateScaleDiv(const QwtScaleDiv& azimuthMap, const QwtScaleDiv& radialMap, const QwtInterval&) override;

    /// Get the margin hint
    virtual int marginHint() const override;

protected:
    /// Draw the rays
    void drawRays(QPainter*,
                  const QRectF&,
                  const QPointF& pole,
                  double radius,
                  const QwtScaleMap& azimuthMap,
                  const QList< double >&) const;
    /// Draw the circles
    void drawCircles(QPainter*, const QRectF&, const QPointF& pole, const QwtScaleMap& radialMap, const QList< double >&) const;

    /// Draw an axis
    void drawAxis(QPainter*, int axisId) const;

private:
    void updateScaleDraws(const QwtScaleMap& azimuthMap,
                          const QwtScaleMap& radialMap,
                          const QPointF& pole,
                          const double radius) const;

private:
    QWT_DECLARE_PRIVATE(QwtPolarGrid)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPolarGrid::DisplayFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPolarGrid::GridAttributes)

#endif
