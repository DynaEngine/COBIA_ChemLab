/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PYPLOT_H
#define QWT_PYPLOT_H

#include "qwt_global.h"
#include "qwt_symbol.h"

#include <qobject.h>
#include <qcolor.h>
#include <qpen.h>
#include <qstring.h>
#include <qvector.h>

class QPointF;
class QRectF;
class QwtFigure;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotBarChart;
class QwtPlotHistogram;
class QwtPlotBoxChart;
class QwtPlotIntervalCurve;
class QwtPlotSpectrogram;
class QwtPlotVectorField;
class QwtPlotTradingCurve;
class QwtPlotGrid;
class QwtPlotMarker;
class QwtPlotZoneItem;
class QwtPlotArrowMarker;
class QwtPlotLegendItem;
class QwtBoxSample;
class QwtOHLCSample;

/**
 * @brief Parsed representation of a matplotlib-style format string
 *
 * @details Parses format strings like "r-o", "b--", "g^:", "ko" etc.
 *          The format string can contain up to three components:
 *          - Color character: b, g, r, c, m, y, k, w
 *          - Marker character: o, s, ^, v, D, x, +, *, .
 *          - Line style: -, --, -., :
 *
 *          If only a marker is specified without a line style (e.g. "ro"),
 *          the line is suppressed (noLine = true), matching matplotlib behavior.
 *
 * @sa QwtPyPlot
 */
struct QWT_EXPORT QwtFormatString
{
    QColor color;
    QwtSymbol::Style marker = QwtSymbol::NoSymbol;
    Qt::PenStyle lineStyle  = Qt::SolidLine;
    bool hasColor           = false;
    bool hasMarker          = false;
    bool hasLineStyle       = false;
    bool noLine             = false;

    // Parse a matplotlib format string
    static QwtFormatString parse(const QString& fmt);
};

/**
 * @brief Matplotlib pyplot-like interface for Qwt plotting
 *
 * @details QwtPyPlot provides a high-level, stateful API inspired by
 *          matplotlib's pyplot module. It wraps a QwtFigure (or a single
 *          QwtPlot) and delegates all operations to the existing Qwt
 *          infrastructure (QwtPlotFactory, QwtPlotStyling, etc.).
 *
 *          The class maintains a "current axes" pointer (like matplotlib's
 *          gca()), so successive calls to plot(), setTitle(), etc. always
 *          operate on the active subplot.
 *
 * @par Example:
 * @code
 * // Create a figure with 2 subplots
 * QwtFigure* fig = new QwtFigure;
 * QwtPyPlot plt(fig);
 *
 * plt.subplot(2, 1, 1);
 * plt.plot({0, 1, 2, 3}, {1, 4, 2, 5}, "r-o", "Temperature");
 * plt.setTitle("Sensor Data");
 * plt.grid(true);
 * plt.legend();
 *
 * plt.subplot(2, 1, 2);
 * plt.bar({10, 20, 30, 40}, "b", "Sales");
 *
 * plt.savefig("output.png", 300);
 * fig->show();
 * @endcode
 *
 * @par Single-plot mode:
 * @code
 * QwtPlot* plot = new QwtPlot;
 * QwtPyPlot plt(plot);
 * plt.plot(x, y, "b--");
 * plt.scatter(x2, y2, 50, "r");
 * plot->show();
 * @endcode
 *
 * @sa QwtFigure, QwtPlot, QwtPlotFactory, QwtPlotStyling
 */
class QWT_EXPORT QwtPyPlot : public QObject
{
    Q_OBJECT

public:
    // Construct from a QwtFigure (multi-subplot mode)
    explicit QwtPyPlot(QwtFigure* figure, QObject* parent = nullptr);

    // Construct from a single QwtPlot (single-plot mode)
    explicit QwtPyPlot(QwtPlot* plot, QObject* parent = nullptr);

    ~QwtPyPlot() override;

    // ---- State management ----

    // Get the current figure (like matplotlib's gcf)
    QwtFigure* gcf() const;

    // Get the current axes (like matplotlib's gca)
    QwtPlot* gca() const;

    // Set the current axes (like matplotlib's sca)
    void sca(QwtPlot* plot);

    // ---- Figure operations ----

    // Create a subplot in a grid layout (1-based index, like matplotlib)
    QwtPlot* subplot(int rows, int cols, int index);

    // Add axes at a normalized rectangle position
    QwtPlot* addAxes(const QRectF& rect = QRectF(0.1, 0.1, 0.8, 0.8));

    // Create a twin Y-axis (like matplotlib's twinx)
    QwtPlot* twinx(QwtPlot* host = nullptr);

    // Create a twin X-axis (like matplotlib's twiny)
    QwtPlot* twiny(QwtPlot* host = nullptr);

    // Apply tight layout to align all subplot axes
    void tightLayout();

    // ---- Plotting methods (operate on gca()) ----

    // Plot y-only data (x = index)
    QwtPlotCurve* plot(const QVector< double >& y, const QString& fmt = QString(), const QString& label = QString());

    // Plot x-y data from separate vectors
    QwtPlotCurve* plot(const QVector< double >& x,
                       const QVector< double >& y,
                       const QString& fmt   = QString(),
                       const QString& label = QString());

    // Plot from QPointF data
    QwtPlotCurve* plot(const QVector< QPointF >& data, const QString& fmt = QString(), const QString& label = QString());

    // Scatter plot (markers only, no lines)
    QwtPlotCurve* scatter(const QVector< double >& x,
                          const QVector< double >& y,
                          double size          = 20,
                          const QString& color = QString(),
                          const QString& label = QString());

    // Bar chart from y-only values (x = index)
    QwtPlotBarChart* bar(const QVector< double >& values, const QString& color = QString(), const QString& label = QString());

    // Bar chart from x-y data with configurable width
    QwtPlotBarChart* bar(const QVector< double >& x,
                         const QVector< double >& values,
                         double width         = 0.8,
                         const QString& color = QString(),
                         const QString& label = QString());

    // Histogram from raw data with automatic binning
    QwtPlotHistogram*
    hist(const QVector< double >& data, int bins = 10, const QString& color = QString(), const QString& label = QString());

    // Box plot from pre-computed box samples
    QwtPlotBoxChart* boxplot(const QVector< QwtBoxSample >& data, const QString& label = QString());

    // Fill the area between two curves
    QwtPlotIntervalCurve* fillBetween(const QVector< double >& x,
                                      const QVector< double >& y1,
                                      const QVector< double >& y2,
                                      const QString& color = QString(),
                                      double alpha         = 0.3);

    // Error bars (symmetric y-error)
    QwtPlotIntervalCurve* errorbar(const QVector< double >& x,
                                   const QVector< double >& y,
                                   const QVector< double >& yerr,
                                   const QString& fmt   = QString(),
                                   const QString& label = QString());

    // Display a 2D matrix as a color-mapped image
    QwtPlotSpectrogram*
    imshow(const QVector< QVector< double > >& data, const QString& cmap = "viridis", double vmin = 0.0, double vmax = 0.0);

    // Draw contour lines from a 2D matrix
    QwtPlotSpectrogram* contour(const QVector< QVector< double > >& data,
                                const QList< double >& levels = {},
                                const QString& cmap           = "viridis");

    // Quiver plot (vector field)
    QwtPlotVectorField* quiver(const QVector< double >& x,
                               const QVector< double >& y,
                               const QVector< double >& u,
                               const QVector< double >& v,
                               const QString& color = QString());

    // Candlestick (OHLC) chart
    QwtPlotTradingCurve* candlestick(const QVector< QwtOHLCSample >& data, const QString& label = QString());

    // ---- Auxiliary elements ----

    // Add or remove a grid
    QwtPlotGrid* grid(bool show = true, bool minor = false);

    // Add a horizontal line at y
    QwtPlotMarker* axhline(double y, const QString& fmt = QString());

    // Add a vertical line at x
    QwtPlotMarker* axvline(double x, const QString& fmt = QString());

    // Add a horizontal colored span between y1 and y2
    QwtPlotZoneItem* axhspan(double y1, double y2, const QString& color = QString(), double alpha = 0.3);

    // Add a vertical colored span between x1 and x2
    QwtPlotZoneItem* axvspan(double x1, double x2, const QString& color = QString(), double alpha = 0.3);

    // Add an arrow annotation from xytext to xy
    QwtPlotArrowMarker* annotate(const QString& text, const QPointF& xy, const QPointF& xytext);

    // Add a legend (in-canvas legend item)
    QwtPlotLegendItem* legend(const QString& loc = "best");

    // ---- Axis configuration ----

    // Set the plot title
    void setTitle(const QString& title);

    // Set the X-axis label
    void setXLabel(const QString& label);

    // Set the Y-axis label
    void setYLabel(const QString& label);

    // Set X-axis limits
    void setXLim(double min, double max);

    // Set Y-axis limits
    void setYLim(double min, double max);

    // Set X-axis scale type ("linear" or "log")
    void setXScale(const QString& scale);

    // Set Y-axis scale type ("linear" or "log")
    void setYScale(const QString& scale);

    // Set custom X-axis tick positions and optional labels
    void setXTicks(const QVector< double >& ticks, const QStringList& labels = {});

    // Set custom Y-axis tick positions and optional labels
    void setYTicks(const QVector< double >& ticks, const QStringList& labels = {});

    // Invert the X-axis
    void invertXAxis();

    // Invert the Y-axis
    void invertYAxis();

    // ---- Appearance ----

    // Set figure background color
    void setFaceColor(const QString& color);

    // Set axes canvas background color
    void setAxesColor(const QString& color);

    // Add a colorbar for a spectrogram
    void colorbar(QwtPlotSpectrogram* spectro = nullptr);

    // ---- Output ----

    // Save figure to file
    bool savefig(const QString& filename, int dpi = -1);

    // Show the figure or plot widget
    void show();

    // ---- Interaction ----

    // Enable/disable canvas panning
    void enablePan(bool enable = true);

    // Enable/disable canvas zooming (rubber-band)
    void enableZoom(bool enable = true);

private:
    // Apply a parsed format string to a curve
    void applyFormat(QwtPlotCurve* curve, const QwtFormatString& f);

    // Apply a parsed format string to a marker
    void applyFormat(QwtPlotMarker* marker, const QwtFormatString& f);

    // Parse a named color string to QColor
    static QColor namedColor(const QString& name);

    // Create a color map by name
    static class QwtLinearColorMap* createColorMap(const QString& name);

    QWT_DECLARE_PRIVATE(QwtPyPlot)
};

#endif  // QWT_PYPLOT_H
