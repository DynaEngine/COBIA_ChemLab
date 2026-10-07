#ifndef QWTSTYLESHEETRECORDER_H
#define QWTSTYLESHEETRECORDER_H
#include "qwt_null_paintdevice.h"

#include <QList>
#include <QVector>
#include <QRectF>
#include <QRectF>
#include <QPainterPath>
#include <QBrush>
#include <QPointF>

/**
 * @brief A paint device that records style sheet information for rendering.
 *
 * QwtStyleSheetRecorder is used to capture style information (borders, backgrounds, etc.)
 * for rendering styled widgets or elements.
 *
 */
class QWT_EXPORT QwtStyleSheetRecorder final : public QwtNullPaintDevice
{
public:
    /// Constructor with specified size
    explicit QwtStyleSheetRecorder(const QSize& size);
    /// Update the paint engine state
    virtual void updateState(const QPaintEngineState& state) override;
    /// Draw rectangles with floating-point coordinates
    virtual void drawRects(const QRectF* rects, int count) override;
    /// Draw rectangles with integer coordinates
    virtual void drawRects(const QRect* rects, int count) override;
    /// Draw a painter path
    virtual void drawPath(const QPainterPath& path) override;
    /// Set corner rectangles from a path
    void setCornerRects(const QPainterPath& path);

protected:
    /// Get the size metrics of the device
    virtual QSize sizeMetrics() const override;

private:
    /// Align corner rectangles to a reference rectangle
    void alignCornerRects(const QRectF& rect);

public:
    /// Clip rectangles for clipping operations
    QVector< QRectF > clipRects;

    /// Border structure containing path and rectangle information
    struct Border
    {
        QList< QPainterPath > pathList;
        QList< QRectF > rectList;
        QRegion clipRegion;
    } border;

    /// Background structure containing path, brush, and origin
    struct Background
    {
        QPainterPath path;
        QBrush brush;
        QPointF origin;
    } background;

private:
    const QSize m_size;

    QPen m_pen;
    QBrush m_brush;
    QPointF m_origin;
};

#endif  // QWTSTYLESHEETRECORDER_H
