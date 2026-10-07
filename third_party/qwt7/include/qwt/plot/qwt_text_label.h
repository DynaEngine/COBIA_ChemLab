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

#ifndef QWT_TEXT_LABEL_H
#define QWT_TEXT_LABEL_H

#include "qwt_global.h"
#include "qwt_text.h"

#include <qframe.h>

class QString;
class QPaintEvent;
class QPainter;

/**
 * @brief A Widget which displays a QwtText
 * @details QwtTextLabel displays a text label supporting rich text formatting
 *          and rotated text. The text can be aligned horizontally and vertically.
 * @sa QwtText
 */

class QWT_EXPORT QwtTextLabel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(int indent READ indent WRITE setIndent)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
    Q_PROPERTY(QString plainText READ plainText WRITE setPlainText)

public:
    // Constructor with parent
    explicit QwtTextLabel(QWidget* parent = nullptr);
    // Constructor with text and parent
    explicit QwtTextLabel(const QwtText&, QWidget* parent = nullptr);
    // Destructor
    ~QwtTextLabel() override;

    // Set the text as plain text
    void setPlainText(const QString&);
    // Return the text as plain text
    QString plainText() const;

public Q_SLOTS:
    // Set the text with auto format detection
    void setText(const QString&, QwtText::TextFormat textFormat = QwtText::AutoText);
    // Set the text
    virtual void setText(const QwtText&);

    // Clear the text
    void clear();

public:
    // Return the text
    const QwtText& text() const;

    // Return the indent
    int indent() const;
    // Set the indent
    void setIndent(int);

    // Return the margin
    int margin() const;
    // Set the margin
    void setMargin(int);

    // Return the size hint
    virtual QSize sizeHint() const override;
    // Return the minimum size hint
    virtual QSize minimumSizeHint() const override;
    // Return the height for a given width
    virtual int heightForWidth(int) const override;

    // Return the rectangle for the text
    QRect textRect() const;

    // Draw the text
    virtual void drawText(QPainter*, const QRectF&);

protected:
    /// Paint event handler
    virtual void paintEvent(QPaintEvent*) override;
    /// Draw the contents
    virtual void drawContents(QPainter*);

private:
    /// Initialize the label
    void init();
    /// Return the default indent
    int defaultIndent() const;

    QWT_DECLARE_PRIVATE(QwtTextLabel)
};

#endif
