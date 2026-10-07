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

#ifndef QWT_PLOT_ITEM_H
#define QWT_PLOT_ITEM_H

#include "qwt_global.h"
#include "qwt_axis_id.h"
#include <qmetatype.h>

class QwtScaleMap;
class QwtScaleDiv;
class QwtPlot;
class QwtText;
class QwtGraphic;
class QwtLegendData;
class QRectF;
class QPainter;
class QString;
template< typename T >
class QList;

/**
 * @brief Base class for items on the plot canvas
 *
 * A plot item is "something", that can be painted on the plot canvas,
 * or only affects the scales of the plot widget. They can be categorized as:
 *
 * - Representator
 *  A "Representator" is an item that represents some sort of data
 *  on the plot canvas. The different representator classes are organized
 *  according to the characteristics of the data:
 *  - QwtPlotMarker
 *    Represents a point or a horizontal/vertical coordinate
 *  - QwtPlotCurve
 *    Represents a series of points
 *  - QwtPlotSpectrogram ( QwtPlotRasterItem )
 *    Represents raster data
 *  - ...
 *
 * - Decorators
 *  A "Decorator" is an item, that displays additional information, that
 *  is not related to any data:
 *  - QwtPlotGrid
 *  - QwtPlotScaleItem
 *  - QwtPlotSvgItem
 *  - ...
 *
 * Depending on the QwtPlotItem::ItemAttribute flags, an item is included
 * into autoscaling or has an entry on the legend.
 *
 * Before misusing the existing item classes it might be better to
 * implement a new type of plot item
 * ( don't implement a watermark as spectrogram ).
 * Deriving a new type of QwtPlotItem primarily means to implement
 * the YourPlotItem::draw() method.
 *
 * @sa The cpuplot example shows the implementation of additional plot items.
 */

class QWT_EXPORT QwtPlotItem
{
public:
    /**
     * @brief Runtime type information
     *
     * RttiValues is used to cast plot items, without
     * having to enable runtime type information of the compiler.
     */
    enum RttiValues
    {
        //! Unspecific value, that can be used, when it doesn't matter
        Rtti_PlotItem = 0,

        //! For QwtPlotGrid
        Rtti_PlotGrid,

        //! For QwtPlotScaleItem
        Rtti_PlotScale,

        //! For QwtPlotLegendItem
        Rtti_PlotLegend,

        //! For QwtPlotMarker
        Rtti_PlotMarker,

        //! For QwtPlotCurve
        Rtti_PlotCurve,

        //! For QwtPlotSpectroCurve
        Rtti_PlotSpectroCurve,

        //! For QwtPlotIntervalCurve
        Rtti_PlotIntervalCurve,

        //! For QwtPlotHistogram
        Rtti_PlotHistogram,

        //! For QwtPlotSpectrogram
        Rtti_PlotSpectrogram,

        //! For QwtPlotGraphicItem, QwtPlotSvgItem
        Rtti_PlotGraphic,

        //! For QwtPlotTradingCurve
        Rtti_PlotTradingCurve,

        //! For QwtPlotBarChart
        Rtti_PlotBarChart,

        //! For QwtPlotMultiBarChart
        Rtti_PlotMultiBarChart,

        //! For QwtPlotShapeItem
        Rtti_PlotShape,

        //! For QwtPlotTextLabel
        Rtti_PlotTextLabel,

        //! For QwtPlotZoneItem
        Rtti_PlotZone,

        //! For QwtPlotVectorField
        Rtti_PlotVectorField,

        //! For QwtPlotArrowMarker
        Rtti_PlotArrowMarker,

        //! Boxplot chart item
        Rtti_PlotBoxChart,

        /*!
           Values >= Rtti_PlotUserItem are reserved for plot items
           not implemented in the Qwt library.
         */
        Rtti_PlotUserItem = 1000
    };

    /**
     * @brief Plot Item Attributes
     *
     * Various aspects of a plot widget depend on the attributes of
     * the attached plot items. If and how a single plot item
     * participates in these updates depends on its attributes.
     *
     * @sa setItemAttribute(), testItemAttribute(), ItemInterest
     */
    enum ItemAttribute
    {
        //! The item is represented on the legend.
        Legend = 0x01,

        /*!
           The boundingRect() of the item is included in the
           autoscaling calculation as long as its width or height
           is >= 0.0.
         */
        AutoScale = 0x02,

        /*!
           The item needs extra space to display something outside
           its bounding rectangle.
           @sa getCanvasMarginHint()
         */
        Margins = 0x04
    };

    Q_DECLARE_FLAGS(ItemAttributes, ItemAttribute)

    /**
     * @brief Plot Item Interests
     *
     * Plot items might depend on the situation of the corresponding plot widget. By enabling an interest the plot item
     * will be notified, when the corresponding attribute of the plot widgets has changed.
     *
     * @sa setItemAttribute(), testItemAttribute(), ItemInterest
     */
    enum ItemInterest
    {
        /**
         * The item is interested in updates of the scales
         * @sa updateScaleDiv()
         */
        ScaleInterest = 0x01,

        /**
         * The item is interested in updates of the legend ( of other items )
         * This flag is intended for items, that want to implement a legend for displaying entries of other plot item.
         *
         * @note If the plot item wants to be represented on a legend enable QwtPlotItem::Legend instead.
         * @sa updateLegend()
         */
        LegendInterest = 0x02
    };

    Q_DECLARE_FLAGS(ItemInterests, ItemInterest)

    /**
     * @brief Render hints
     */
    enum RenderHint
    {
        //! Enable antialiasing
        RenderAntialiased = 0x1
    };

    Q_DECLARE_FLAGS(RenderHints, RenderHint)

    /// Default constructor
    explicit QwtPlotItem();
    /// Constructor with title as QString
    explicit QwtPlotItem(const QString& title);
    /// Constructor with title as QwtText
    explicit QwtPlotItem(const QwtText& title);

    /// Destructor
    virtual ~QwtPlotItem();

    /// Attach the item to a plot
    virtual void attach(QwtPlot* plot);
    /// Detach the item from the plot
    void detach();

    /// Get the plot the item is attached to
    QwtPlot* plot() const;

    /// Set the title using a QString
    void setTitle(const QString& title);
    /// Set the title using a QwtText
    void setTitle(const QwtText& title);
    /// Get the title
    const QwtText& title() const;

    /// Runtime type information
    virtual int rtti() const;

    /// Set an item attribute
    void setItemAttribute(ItemAttribute, bool on = true);
    /// Test an item attribute
    bool testItemAttribute(ItemAttribute) const;

    /// Set an item interest
    void setItemInterest(ItemInterest, bool on = true);
    /// Test an item interest
    bool testItemInterest(ItemInterest) const;

    /// Set a render hint
    void setRenderHint(RenderHint, bool on = true);
    /// Test a render hint
    bool testRenderHint(RenderHint) const;

    /// Set the number of render threads
    void setRenderThreadCount(uint numThreads);
    /// Get the number of render threads
    uint renderThreadCount() const;

    /// Set the legend icon size
    void setLegendIconSize(const QSize&);
    /// Get the legend icon size
    QSize legendIconSize() const;

    /// Get the z-value
    double z() const;
    /// Set the z-value
    void setZ(double z);

    /// Show the item
    void show();
    /// Hide the item
    void hide();
    /// Set the visibility
    virtual void setVisible(bool);
    /// Check if the item is visible
    bool isVisible() const;

    /// Set both axes
    void setAxes(QwtAxisId xAxis, QwtAxisId yAxis);

    /// Set the x-axis
    void setXAxis(QwtAxisId);
    /// Get the x-axis
    QwtAxisId xAxis() const;

    /// Set the y-axis
    void setYAxis(QwtAxisId);
    /// Get the y-axis
    QwtAxisId yAxis() const;

    /// Notify the plot that the item has changed
    virtual void itemChanged();
    /// Notify the item that the legend has changed
    virtual void legendChanged();

    /**
     * @brief Draw the item
     *
     * @param painter Painter
     * @param xMap Maps x-values into pixel coordinates.
     * @param yMap Maps y-values into pixel coordinates.
     * @param canvasRect Contents rect of the canvas in painter coordinates
     */
    virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const = 0;

    /// Get the bounding rectangle
    virtual QRectF boundingRect() const;

    /// Get margin hints for the canvas
    virtual void getCanvasMarginHint(const QwtScaleMap& xMap,
                                     const QwtScaleMap& yMap,
                                     const QRectF& canvasRect,
                                     double& left,
                                     double& top,
                                     double& right,
                                     double& bottom) const;

    /// Update the item with new scale divisions
    virtual void updateScaleDiv(const QwtScaleDiv&, const QwtScaleDiv&);

    /// Update the item with changes of the legend
    virtual void updateLegend(const QwtPlotItem*, const QList< QwtLegendData >&);

    /// Calculate the scale rectangle
    QRectF scaleRect(const QwtScaleMap&, const QwtScaleMap&) const;
    /// Calculate the paint rectangle
    QRectF paintRect(const QwtScaleMap&, const QwtScaleMap&) const;

    /// Return legend data
    virtual QList< QwtLegendData > legendData() const;

    /// Return a legend icon
    virtual QwtGraphic legendIcon(int index, const QSizeF&) const;

protected:
    /// Create a default icon
    QwtGraphic defaultIcon(const QBrush&, const QSizeF&) const;

private:
    QwtPlotItem(const QwtPlotItem&)            = delete;
    QwtPlotItem& operator=(const QwtPlotItem&) = delete;
    QWT_DECLARE_PRIVATE(QwtPlotItem)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotItem::ItemAttributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotItem::ItemInterests)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtPlotItem::RenderHints)

Q_DECLARE_METATYPE(QwtPlotItem*)

#endif
