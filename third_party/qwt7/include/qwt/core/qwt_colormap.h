/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_COLOR_MAP_H
#define QWT_COLOR_MAP_H

#include "qwtcore_global.h"
#include <qcolor.h>

#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief QwtColorMap is used to map values into colors.
 * @details For displaying 3D data on a 2D plane the 3rd dimension is often
 *          displayed using colors, like f.e in a spectrogram.
 *          Each color map is optimized to return colors for only one of the
 *          following image formats:
 *          - QImage::Format_Indexed8
 *          - QImage::Format_ARGB32
 * @sa QwtPlotSpectrogram, QwtScaleWidget
 */

class QWTCORE_EXPORT QwtColorMap
{
public:
    /*!
        Format for color mapping
        @sa rgb(), colorIndex(), colorTable()
     */

    enum Format
    {
        //! The map is intended to map into RGB values.
        RGB,

        /*!
           Map values into 8 bit values, that
           are used as indexes into the color table.

           Indexed color maps are used to generate QImage::Format_Indexed8
           images. The calculation of the color index is usually faster
           and the resulting image has a lower memory footprint.

           @sa colorIndex(), colorTable()
         */
        Indexed
    };

    explicit QwtColorMap(Format = QwtColorMap::RGB);
    virtual ~QwtColorMap();

    void setFormat(Format);
    Format format() const;

    /// Map a value of a given interval into a RGB value.
    virtual QRgb rgb(double vMin, double vMax, double value) const = 0;

    virtual uint colorIndex(int numColors, double vMin, double vMax, double value) const;

    QColor color(double vMin, double vMax, double value) const;
    virtual QVector< QRgb > colorTable(int numColors) const;
    virtual QVector< QRgb > colorTable256() const;

private:
    QwtColorMap(const QwtColorMap&)            = delete;
    QwtColorMap& operator=(const QwtColorMap&) = delete;

    Format m_format;
};

/**
 * @brief QwtLinearColorMap builds a color map from color stops.
 * @details A color stop is a color at a specific position. The valid
 *          range for the positions is [0.0, 1.0]. When mapping a value
 *          into a color it is translated into this interval according to mode().
 */
class QWTCORE_EXPORT QwtLinearColorMap : public QwtColorMap
{
public:
    /*!
       Mode of color map
       @sa setMode(), mode()
     */
    enum Mode
    {
        //! Return the color from the next lower color stop
        FixedColors,

        //! Interpolating the colors of the adjacent stops.
        ScaledColors
    };

    explicit QwtLinearColorMap(QwtColorMap::Format = QwtColorMap::RGB);

    QwtLinearColorMap(const QColor& from, const QColor& to, QwtColorMap::Format = QwtColorMap::RGB);

    ~QwtLinearColorMap() override;

    void setMode(Mode);
    Mode mode() const;

    void setColorInterval(const QColor& color1, const QColor& color2);
    void addColorStop(double value, const QColor&);
    QVector< double > stopPos() const;
    QVector< QColor > stopColors() const;
    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb(double vMin, double vMax, double value) const override;

    virtual uint colorIndex(int numColors, double vMin, double vMax, double value) const override;

    class ColorStops;

private:
    QWT_DECLARE_PRIVATE(QwtLinearColorMap)
};

/**
 * @brief QwtAlphaColorMap varies the alpha value of a color.
 */
class QWTCORE_EXPORT QwtAlphaColorMap : public QwtColorMap
{
public:
    explicit QwtAlphaColorMap(const QColor& = QColor(Qt::gray));
    ~QwtAlphaColorMap() override;

    void setAlphaInterval(int alpha1, int alpha2);

    int alpha1() const;
    int alpha2() const;

    void setColor(const QColor&);
    QColor color() const;

    virtual QRgb rgb(double vMin, double vMax, double value) const override;

private:
    QWT_DECLARE_PRIVATE(QwtAlphaColorMap)
};

/**
 * @brief QwtHueColorMap varies the hue value of the HSV color model.
 * @details QwtHueColorMap can be used to set up a color map easily, that runs cyclic over
 *          all colors. Each cycle has 360 different steps.
 *          The values for value and saturation are in the range of 0 to 255 and doesn't
 *          depend on the data value to be mapped.
 * @sa QwtSaturationValueColorMap
 */
class QWTCORE_EXPORT QwtHueColorMap : public QwtColorMap
{
public:
    explicit QwtHueColorMap(QwtColorMap::Format = QwtColorMap::RGB);
    ~QwtHueColorMap() override;

    void setHueInterval(int hue1, int hue2);  // direction ?
    void setSaturation(int saturation);
    void setValue(int value);
    void setAlpha(int alpha);

    int hue1() const;
    int hue2() const;
    int saturation() const;
    int value() const;
    int alpha() const;

    virtual QRgb rgb(double vMin, double vMax, double value) const override;

private:
    QWT_DECLARE_PRIVATE(QwtHueColorMap)
};

/**
 * @brief QwtSaturationValueColorMap varies the saturation and/or value for a given hue in the HSV color model.
 * @details Value and saturation are in the range of 0 to 255 while hue is in the range of 0 to 359.
 * @sa QwtHueColorMap
 */
class QWTCORE_EXPORT QwtSaturationValueColorMap : public QwtColorMap
{
public:
    QwtSaturationValueColorMap();
    ~QwtSaturationValueColorMap() override;

    void setHue(int hue);
    void setSaturationInterval(int sat1, int sat2);
    void setValueInterval(int value1, int value2);
    void setAlpha(int alpha);

    int hue() const;
    int saturation1() const;
    int saturation2() const;
    int value1() const;
    int value2() const;
    int alpha() const;

    virtual QRgb rgb(double vMin, double vMax, double value) const override;

private:
    QWT_DECLARE_PRIVATE(QwtSaturationValueColorMap)
};

// Map a value into a color.
inline QColor QwtColorMap::color(double vMin, double vMax, double value) const
{
    return QColor::fromRgba(rgb(vMin, vMax, value));
}

// Return the intended format of the color map.
inline QwtColorMap::Format QwtColorMap::format() const
{
    return m_format;
}

#endif
