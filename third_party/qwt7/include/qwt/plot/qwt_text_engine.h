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

#ifndef QWT_TEXT_ENGINE_H
#define QWT_TEXT_ENGINE_H

#include "qwt_global.h"
#include <qsize.h>

class QFont;
class QRectF;
class QString;
class QPainter;

/**
 * @brief Abstract base class for rendering text strings
 * @details A text engine is responsible for rendering texts for a
 *          specific text format. They are used by QwtText to render a text.
 * @sa QwtText::setTextEngine()
 */

class QWT_EXPORT QwtTextEngine
{
public:
    // Virtual destructor
    virtual ~QwtTextEngine();

    // Find the height for a given width
    virtual double heightForWidth(const QFont& font, int flags, const QString& text, double width) const = 0;

    // Return the size needed to render text
    virtual QSizeF textSize(const QFont& font, int flags, const QString& text) const = 0;

    // Test if a string can be rendered by this text engine
    virtual bool mightRender(const QString& text) const = 0;

    // Return margins around the texts
    virtual void
    textMargins(const QFont& font, const QString& text, double& left, double& right, double& top, double& bottom) const = 0;

    // Draw the text in a clipping rectangle
    virtual void draw(QPainter* painter, const QRectF& rect, int flags, const QString& text) const = 0;

protected:
    /// Protected constructor
    QwtTextEngine();

private:
    QwtTextEngine(const QwtTextEngine&)            = delete;
    QwtTextEngine& operator=(const QwtTextEngine&) = delete;
};

/**
 * @brief A text engine for plain texts
 * @details QwtPlainTextEngine renders texts using the basic Qt classes
 *          QPainter and QFontMetrics.
 * @sa QwtRichTextEngine
 */
class QWT_EXPORT QwtPlainTextEngine : public QwtTextEngine
{
public:
    // Constructor
    QwtPlainTextEngine();
    // Destructor
    ~QwtPlainTextEngine() override;

    // Calculate height for a given width
    virtual double heightForWidth(const QFont& font, int flags, const QString& text, double width) const override;

    // Return the size needed to render text
    virtual QSizeF textSize(const QFont& font, int flags, const QString& text) const override;

    // Draw the text
    virtual void draw(QPainter*, const QRectF& rect, int flags, const QString& text) const override;

    // Test if a string can be rendered
    virtual bool mightRender(const QString&) const override;

    // Return text margins
    virtual void
    textMargins(const QFont&, const QString&, double& left, double& right, double& top, double& bottom) const override;

private:
    QWT_DECLARE_PRIVATE(QwtPlainTextEngine)
};

#ifndef QT_NO_RICHTEXT

/**
 * @brief A text engine for Qt rich texts
 * @details QwtRichTextEngine renders Qt rich texts using the classes
 *          of the Scribe framework of Qt.
 * @sa QwtPlainTextEngine
 */
class QWT_EXPORT QwtRichTextEngine : public QwtTextEngine
{
public:
    // Constructor
    QwtRichTextEngine();

    // Calculate height for a given width
    virtual double heightForWidth(const QFont& font, int flags, const QString& text, double width) const override;

    // Return the size needed to render text
    virtual QSizeF textSize(const QFont& font, int flags, const QString& text) const override;

    // Draw the text
    virtual void draw(QPainter*, const QRectF& rect, int flags, const QString& text) const override;

    // Test if a string can be rendered
    virtual bool mightRender(const QString&) const override;

    // Return text margins
    virtual void
    textMargins(const QFont&, const QString&, double& left, double& right, double& top, double& bottom) const override;

private:
    QString taggedText(const QString&, int flags) const;
};

#endif  // !QT_NO_RICHTEXT

#endif
