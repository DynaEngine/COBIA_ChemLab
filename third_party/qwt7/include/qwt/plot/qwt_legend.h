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

#ifndef QWT_LEGEND_H
#define QWT_LEGEND_H

#include "qwt_global.h"
#include "qwt_abstract_legend.h"
#include "qwt_legend_data.h"

#include <qvariant.h>

class QScrollBar;

/**
 * @brief The legend widget
 * @details The QwtLegend widget is a tabular arrangement of legend items. Legend
 *          items might be any type of widget, but in general they will be
 *          a QwtLegendLabel.
 * @sa QwtLegendLabel, QwtPlotItem, QwtPlot
 */

class QWT_EXPORT QwtLegend : public QwtAbstractLegend
{
    Q_OBJECT

public:
    // Constructor for QwtLegend
    explicit QwtLegend(QWidget* parent = nullptr);
    // Destructor
    ~QwtLegend() override;

    // Set the maximum number of columns
    void setMaxColumns(uint numColums);
    // Return the maximum number of columns
    uint maxColumns() const;

    // Set the default mode for legend items
    void setDefaultItemMode(QwtLegendData::Mode);
    // Return the default mode for legend items
    QwtLegendData::Mode defaultItemMode() const;

    // Return the contents widget
    QWidget* contentsWidget();
    // Return the contents widget (const version)
    const QWidget* contentsWidget() const;

    // Return the legend widget for a specific item
    QWidget* legendWidget(const QVariant&) const;
    // Return all legend widgets for a specific item
    QList< QWidget* > legendWidgets(const QVariant&) const;

    // Return the item info for a specific widget
    QVariant itemInfo(const QWidget*) const;

    // Event filter for event handling
    virtual bool eventFilter(QObject*, QEvent*) override;

    // Return the size hint
    virtual QSize sizeHint() const override;
    // Return the height for a given width
    virtual int heightForWidth(int w) const override;

    // Return the horizontal scroll bar
    QScrollBar* horizontalScrollBar() const;
    // Return the vertical scroll bar
    QScrollBar* verticalScrollBar() const;

    // Render the legend to a painter
    virtual void renderLegend(QPainter*, const QRectF&, bool fillBackground) const override;

    // Render a single legend item
    virtual void renderItem(QPainter*, const QWidget*, const QRectF&, bool fillBackground) const;

    // Check if the legend is empty
    virtual bool isEmpty() const override;
    // Return the scroll extent for a specific orientation
    virtual int scrollExtent(Qt::Orientation) const override;

Q_SIGNALS:
    /**
     * @brief Signal emitted when the user clicks on a legend label in Clickable mode
     * @param itemInfo Info for the item of the selected legend item
     * @param index Index of the legend label in the list of widgets associated with the plot item
     * @note Clicks are disabled as default
     * @sa setDefaultItemMode(), defaultItemMode(), QwtPlot::itemToInfo()
     */
    void clicked(const QVariant& itemInfo, int index);

    /**
     * @brief Signal emitted when the user clicks on a legend label in Checkable mode
     * @param itemInfo Info for the item of the selected legend label
     * @param on True when the legend label is checked
     * @param index Index of the legend label in the list of widgets associated with the plot item
     * @note Clicks are disabled as default
     * @sa setDefaultItemMode(), defaultItemMode(), QwtPlot::itemToInfo()
     */
    void checked(const QVariant& itemInfo, bool on, int index);

public Q_SLOTS:
    virtual void updateLegend(const QVariant&, const QList< QwtLegendData >&) override;

protected Q_SLOTS:
    void itemClicked();
    void itemChecked(bool);

protected:
    virtual QWidget* createWidget(const QwtLegendData&) const;
    virtual void updateWidget(QWidget*, const QwtLegendData&);

private:
    void updateTabOrder();

    QWT_DECLARE_PRIVATE(QwtLegend)
};

#endif
