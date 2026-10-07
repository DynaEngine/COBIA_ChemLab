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

#ifndef QWT_DYNGRID_LAYOUT_H
#define QWT_DYNGRID_LAYOUT_H

#include "qwt_global.h"
#include <qlayout.h>

template< typename T >
class QList;

/**
 * @brief Dynamic grid layout that adjusts columns and rows to the current size
 *
 * @details QwtDynGridLayout takes the space it gets, divides it up into rows and
 * columns, and puts each of the widgets it manages into the correct cell(s).
 * It lays out as many number of columns as possible (limited by maxColumns()).
 *
 *
 */
class QWT_EXPORT QwtDynGridLayout : public QLayout
{
    Q_OBJECT
public:
    // Constructor with parent widget, margin and spacing
    explicit QwtDynGridLayout(QWidget*, int margin = 0, int spacing = -1);
    // Constructor with spacing only
    explicit QwtDynGridLayout(int spacing = -1);

    // Destructor
    ~QwtDynGridLayout() override;

    // Invalidate all internal caches
    virtual void invalidate() override;

    // Set the maximum number of columns
    void setMaxColumns(uint maxColumns);
    // Get the maximum number of columns
    uint maxColumns() const;

    // Get the number of rows in the current layout
    uint numRows() const;
    // Get the number of columns in the current layout
    uint numColumns() const;

    // Add a layout item to the next free position
    virtual void addItem(QLayoutItem*) override;

    // Get the item at a specific index
    virtual QLayoutItem* itemAt(int index) const override;
    // Remove and return the item at a specific index
    virtual QLayoutItem* takeAt(int index) override;
    // Get the number of items in the layout
    virtual int count() const override;

    // Set the expanding directions for the layout
    void setExpandingDirections(Qt::Orientations);
    // Get the expanding directions for the layout
    virtual Qt::Orientations expandingDirections() const override;
    // Calculate geometries for items with given number of columns
    QList< QRect > layoutItems(const QRect&, uint numColumns) const;

    // Get the maximum width of all layout items
    virtual int maxItemWidth() const;

    // Set the geometry for the layout
    virtual void setGeometry(const QRect&) override;

    // Check if the layout has height for width
    virtual bool hasHeightForWidth() const override;
    // Get the preferred height for a given width
    virtual int heightForWidth(int) const override;

    // Get the size hint for the layout
    virtual QSize sizeHint() const override;

    // Check if the layout is empty
    virtual bool isEmpty() const override;
    // Get the number of layout items
    uint itemCount() const;

    // Calculate the number of columns for a given width
    virtual uint columnsForWidth(int width) const;

protected:
    void layoutGrid(uint numColumns, QVector< int >& rowHeight, QVector< int >& colWidth) const;

    void stretchGrid(const QRect& rect, uint numColumns, QVector< int >& rowHeight, QVector< int >& colWidth) const;

private:
    void init();
    int maxRowWidth(int numColumns) const;

    QWT_DECLARE_PRIVATE(QwtDynGridLayout)
};

#endif
