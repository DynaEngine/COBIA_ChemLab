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

#ifndef QWT_LEGEND_LABEL_H
#define QWT_LEGEND_LABEL_H

#include "qwt_global.h"
#include "qwt_text_label.h"
#include "qwt_legend_data.h"

class QwtText;

/**
 * @brief A widget representing an item on a QwtLegend
 * @details QwtLegendLabel is used to display legend items in a QwtLegend.
 *          It can show an icon and a text label, and supports different
 *          interaction modes (ReadOnly, Clickable, Checkable).
 * @sa QwtLegend, QwtLegendData
 */
class QWT_EXPORT QwtLegendLabel : public QwtTextLabel
{
    Q_OBJECT
public:
    // Constructor for QwtLegendLabel
    explicit QwtLegendLabel(QWidget* parent = nullptr);

    // Destructor for QwtLegendLabel
    ~QwtLegendLabel() override;

    // Set the legend data
    void setData(const QwtLegendData&);

    // Return the legend data
    const QwtLegendData& data() const;

    // Set the item mode
    void setItemMode(QwtLegendData::Mode);

    // Return the item mode
    QwtLegendData::Mode itemMode() const;

    // Set the spacing between icon and text
    void setSpacing(int spacing);

    // Return the spacing
    int spacing() const;

    // Set the text label
    virtual void setText(const QwtText&) override;

    // Set the icon
    void setIcon(const QPixmap&);

    // Return the icon
    QPixmap icon() const;

    // Return the size hint
    virtual QSize sizeHint() const override;

    // Return whether the item is checked
    bool isChecked() const;

public Q_SLOTS:
    // Set whether the item is checked
    void setChecked(bool on);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the legend item has been clicked
     */
    void clicked();

    /**
     * @brief Signal emitted when the legend item has been pressed
     */
    void pressed();

    /**
     * @brief Signal emitted when the legend item has been released
     */
    void released();

    /**
     * @brief Signal emitted when the legend item has been toggled
     * @param on True if checked, false otherwise
     */
    void checked(bool);

protected:
    /// Set whether the button is down (English only)
    void setDown(bool);

    /// Return whether the button is down (English only)
    bool isDown() const;

    /// Handle paint events (English only)
    virtual void paintEvent(QPaintEvent*) override;

    /// Handle mouse press events (English only)
    virtual void mousePressEvent(QMouseEvent*) override;

    /// Handle mouse release events (English only)
    virtual void mouseReleaseEvent(QMouseEvent*) override;

    /// Handle key press events (English only)
    virtual void keyPressEvent(QKeyEvent*) override;

    /// Handle key release events (English only)
    virtual void keyReleaseEvent(QKeyEvent*) override;

private:
    QWT_DECLARE_PRIVATE(QwtLegendLabel)
};

#endif
