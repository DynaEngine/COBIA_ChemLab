#ifndef QWT_PLOT_TRANSPARENT_CANVAS_H
#define QWT_PLOT_TRANSPARENT_CANVAS_H

#include "qwt_global.h"
#include "qwt_plot_abstract_canvas.h"
#include <qframe.h>

/**
 * @brief A transparent canvas for QwtPlot
 * @details QwtPlotTransparentCanvas provides a transparent canvas for QwtPlot
 *          that allows the background to show through.
 */
class QWT_EXPORT QwtPlotTransparentCanvas : public QFrame, public QwtPlotAbstractCanvas
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     */
    explicit QwtPlotTransparentCanvas(QwtPlot* plot = nullptr);
    /**
     * @brief Destructor
     */
    ~QwtPlotTransparentCanvas() override;
public Q_SLOTS:
    /**
     * @brief Replot the canvas
     */
    virtual void replot();

protected:
    /**
     * @brief Paint event handler
     */
    virtual void paintEvent(QPaintEvent* event) override;
    /**
     * @brief Draw the border
     */
    virtual void drawBorder(QPainter* painter) override;
    /**
     * @brief Get the border path
     */
    virtual QPainterPath borderPath(const QRect& rect) const;
};

#endif  // QWT_PLOT_TRANSPARENT_CANVAS_H
