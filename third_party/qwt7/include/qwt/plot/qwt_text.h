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

#ifndef QWT_TEXT_H
#define QWT_TEXT_H

#include "qwt_global.h"
#include <qmetatype.h>

class QFont;
class QString;
class QColor;
class QPen;
class QBrush;
class QSizeF;
class QRectF;
class QPainter;
class QwtTextEngine;

/**
 * @brief A class representing a text
 *
 * A QwtText is a text including a set of attributes how to render it.
 *
 * - Format\n
 *  A text might include control sequences (f.e tags) describing
 *  how to render it. Each format (f.e MathML, TeX, Qt Rich Text)
 *  has its own set of control sequences, that can be handles by
 *  a special QwtTextEngine for this format.
 * - Background\n
 *  A text might have a background, defined by a QPen and QBrush
 *  to improve its visibility. The corners of the background might
 *  be rounded.
 * - Font\n
 *  A text might have an individual font.
 * - Color\n
 *  A text might have an individual color.
 * - Render Flags\n
 *  Flags from Qt::AlignmentFlag and Qt::TextFlag used like in
 *  QPainter::drawText().
 *
 * @sa QwtTextEngine, QwtTextLabel
 *
 */

class QWT_EXPORT QwtText
{
public:
    /**
     * @brief Text format
     *
     * The text format defines the QwtTextEngine, that is used to render
     * the text.
     *
     * @sa QwtTextEngine, setTextEngine()
     *
     */
    enum TextFormat
    {
        /**
         * The text format is determined using QwtTextEngine::mightRender() for
         * all available text engines in increasing order > PlainText.
         * If none of the text engines can render the text is rendered
         * like QwtText::PlainText.
         */
        AutoText = 0,

        //! Draw the text as it is, using a QwtPlainTextEngine.
        PlainText,

        //! Use the Scribe framework (Qt Rich Text) to render the text.
        RichText,

        /**
         * Use a MathML (http://en.wikipedia.org/wiki/MathML) render engine
         * to display the text. In earlier versions of Qwt such an engine
         * was included - since Qwt 6.2 it can be found here:
         * https://github.com/uwerat/qwt-mml-dev
         *
         * To enable MathML support the following code needs to be added to the
         * application:
         *
         * @code
         * QwtText::setTextEngine( QwtText::MathMLText, new QwtMathMLTextEngine() );
         * @endcode
         */
        MathMLText,

        /**
         * Use a TeX (http://en.wikipedia.org/wiki/TeX) render engine
         * to display the text ( not implemented yet ).
         */
        TeXText,

        /**
         * The number of text formats can be extended using setTextEngine.
         * Formats >= QwtText::OtherFormat are not used by Qwt.
         */
        OtherFormat = 100
    };

    /**
     * @brief Paint Attributes
     *
     * Font and color and background are optional attributes of a QwtText.
     * The paint attributes hold the information, if they are set.
     *
     */
    enum PaintAttribute
    {
        //! The text has an individual font.
        PaintUsingTextFont = 0x01,

        //! The text has an individual color.
        PaintUsingTextColor = 0x02,

        //! The text has an individual background.
        PaintBackground = 0x04
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    /**
     * @brief Layout Attributes
     * The layout attributes affects some aspects of the layout of the text.
     *
     */
    enum LayoutAttribute
    {
        /**
         * Layout the text without its margins. This mode is useful if a
         * text needs to be aligned accurately, like the tick labels of a scale.
         * If QwtTextEngine::textMargins is not implemented for the format
         * of the text, MinimumLayout has no effect.
         */
        MinimumLayout = 0x01
    };

    Q_DECLARE_FLAGS(LayoutAttributes, LayoutAttribute)

    // Default constructor
    QwtText();
    // Constructor with text and format
    QwtText(const QString&, TextFormat textFormat = AutoText);
    // Copy constructor
    QwtText(const QwtText&);
    // Move constructor
    QwtText(QwtText&&) noexcept;

    // Destructor
    ~QwtText();

    // Assignment operator
    QwtText& operator=(const QwtText&);
    // Move assignment
    QwtText& operator=(QwtText&&) noexcept;

    // Equality operator
    bool operator==(const QwtText&) const;
    // Inequality operator
    bool operator!=(const QwtText&) const;

    // Set text content and format
    void setText(const QString&, QwtText::TextFormat textFormat = AutoText);
    // Get text content
    QString text() const;

    // Check if text is null
    bool isNull() const;
    // Check if text is empty
    bool isEmpty() const;

    // Set font
    void setFont(const QFont&);
    // Get font
    QFont font() const;

    // Get used font (with fallback)
    QFont usedFont(const QFont&) const;

    // Get the current format
    TextFormat format() const;

    // Set render flags
    void setRenderFlags(int);
    // Get render flags
    int renderFlags() const;

    // Set text color
    void setColor(const QColor&);
    // Get text color
    QColor color() const;

    // Get used color (with fallback)
    QColor usedColor(const QColor&) const;

    // Set border radius
    void setBorderRadius(double);
    // Get border radius
    double borderRadius() const;

    // Set border pen
    void setBorderPen(const QPen&);
    // Get border pen
    QPen borderPen() const;

    // Set background brush
    void setBackgroundBrush(const QBrush&);
    // Get background brush
    QBrush backgroundBrush() const;

    // Set paint attribute
    void setPaintAttribute(PaintAttribute, bool on = true);
    // Test paint attribute
    bool testPaintAttribute(PaintAttribute) const;

    // Set layout attribute
    void setLayoutAttribute(LayoutAttribute, bool on = true);
    // Test layout attribute
    bool testLayoutAttribute(LayoutAttribute) const;

    // Get height for width
    double heightForWidth(double width) const;
    // Get height for width with font
    double heightForWidth(double width, const QFont&) const;

    // Get text size
    QSizeF textSize() const;
    // Get text size with font
    QSizeF textSize(const QFont&) const;

    // Draw text into rectangle
    void draw(QPainter* painter, const QRectF& rect) const;

    // Get text engine for text and format
    static const QwtTextEngine* textEngine(const QString& text, QwtText::TextFormat = AutoText);

    // Get text engine for format
    static const QwtTextEngine* textEngine(QwtText::TextFormat);
    // Set text engine for format
    static void setTextEngine(QwtText::TextFormat, QwtTextEngine*);

private:
    QWT_DECLARE_PRIVATE(QwtText)

    class LayoutCache;
    LayoutCache* m_layoutCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QwtText::PaintAttributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QwtText::LayoutAttributes)

Q_DECLARE_METATYPE(QwtText)

#endif
