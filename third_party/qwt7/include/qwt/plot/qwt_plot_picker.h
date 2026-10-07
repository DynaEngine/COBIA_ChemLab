/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 2024   ChenZongYan <czy.t@163.com>
 *****************************************************************************/

#ifndef QWT_PLOT_PICKER_H
#define QWT_PLOT_PICKER_H

#include "qwt_global.h"
#include "qwt_picker.h"
#include "qwt_axis_id.h"

class QwtPlot;
class QPointF;
class QRectF;

#if QT_VERSION < 0x060000
template< typename T >
class QVector;
#endif

/**
 * @brief QwtPlotPicker provides selections on a plot canvas
 * @details QwtPlotPicker is a QwtPicker tailored for selections on
 *          a plot canvas. It is set to a x-Axis and y-Axis and
 *          translates all pixel coordinates into this coordinate system.
 */
class QWT_EXPORT QwtPlotPicker : public QwtPicker
{
    Q_OBJECT

public:
    //! Constructor
    explicit QwtPlotPicker(QWidget* canvas);

    //! Destructor
    ~QwtPlotPicker() override;

    //! Constructor with axes
    explicit QwtPlotPicker(QwtAxisId xAxisId, QwtAxisId yAxisId, QWidget*);

    //! Constructor with axes, rubber band and tracker mode
    explicit QwtPlotPicker(QwtAxisId xAxisId, QwtAxisId yAxisId, RubberBand rubberBand, DisplayMode trackerMode, QWidget*);

    //! Set the x and y axes
    virtual void setAxes(QwtAxisId xAxisId, QwtAxisId yAxisId);

    //! Get the x axis
    QwtAxisId xAxis() const;

    //! Get the y axis
    QwtAxisId yAxis() const;

    //! Get the plot widget
    QwtPlot* plot();

    //! Get the plot widget (const version)
    const QwtPlot* plot() const;

    //! Get the canvas widget
    QWidget* canvas();

    //! Get the canvas widget (const version)
    const QWidget* canvas() const;

Q_SIGNALS:

    /**
     * @brief Signal emitted in case of QwtPickerMachine::PointSelection
     * @param pos Selected point
     */
    void selected(const QPointF& pos);

    /**
     * @brief Signal emitted in case of QwtPickerMachine::RectSelection
     * @param rect Selected rectangle
     */
    void selected(const QRectF& rect);

    /**
     * @brief Signal emitting the selected points at the end of a selection
     * @param pa Selected points
     */
    void selected(const QVector< QPointF >& pa);

    /**
     * @brief Signal emitted when a point has been appended to the selection
     * @param pos Position of the appended point
     * @sa append(), moved()
     */
    void appended(const QPointF& pos);

    /**
     * @brief Signal emitted whenever the last appended point of the selection has been moved
     * @param pos Position of the moved last point of the selection
     * @sa move(), appended()
     */
    void moved(const QPointF& pos);

protected:
    //! Get the scale rectangle
    QRectF scaleRect() const;

    //! Inverse transform a rectangle
    QRectF invTransform(const QRect&) const;

    //! Transform a rectangle
    QRect transform(const QRectF&) const;

    //! Inverse transform a point
    QPointF invTransform(const QPoint&) const;

    //! Transform a point
    QPoint transform(const QPointF&) const;

    //! Get the tracker text for a point
    virtual QwtText trackerText(const QPoint&) const override;

    //! Get the tracker text for a point (floating point)
    virtual QwtText trackerTextF(const QPointF&) const;

    //! Move the selection
    virtual void move(const QPoint&) override;

    //! Append a point to the selection
    virtual void append(const QPoint&) override;

    //! End the selection
    virtual bool end(bool ok = true) override;

private:
    QWT_DECLARE_PRIVATE(QwtPlotPicker)
};

#endif
