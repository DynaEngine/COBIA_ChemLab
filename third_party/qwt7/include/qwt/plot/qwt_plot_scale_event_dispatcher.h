#ifndef QWTPLOTSCALEEVENTDISPATCHER_H
#define QWTPLOTSCALEEVENTDISPATCHER_H
#include <QObject>
#include "qwt_global.h"
#include "qwt_axis_id.h"
class QMouseEvent;
class QWheelEvent;
class QwtPlot;
class QwtScaleWidget;
/**
 * @brief Event filter for parasitic plots, handling axis interactions
 * @details Since parasitic plots are child windows of host plots, events from multiple parasitic
 *          plots' widgets cannot be passed to other parasitic plots' widgets.
 *          For example, with 2 parasitic plots, their axis areas overlap. The top-level parasitic
 *          plot's axis widget has the same size as the lower-level and host plot's axis widgets.
 *          Only the top-level parasitic axis widget receives events. Even if ignored, events only
 *          fall to the current parasitic plot window, not the next level's axis widget.
 *          This class is designed to solve this problem. Axis actions are executed here as event
 *          handlers rather than event propagators.
 *
 */
class QWT_EXPORT QwtPlotScaleEventDispatcher : public QObject
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtPlotScaleEventDispatcher)
public:
    /// Constructor
    explicit QwtPlotScaleEventDispatcher(QwtPlot* plot, QObject* par = nullptr);
    /// Destructor
    ~QwtPlotScaleEventDispatcher() override;
    /// Check if enabled
    bool isEnable() const;
    /// Find axis ID corresponding to QwtScaleWidget
    static QwtAxisId findAxisIdByScaleWidget(const QwtPlot* plot, const QwtScaleWidget* scaleWidget);
public Q_SLOTS:
    /// Update cache
    void updateCache();
    /// Set enabled state
    void setEnable(bool on = true);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* e) override;
    // Update data
    void rebuildCache();
    // Handle various mouse events
    virtual bool handleMousePress(QwtPlot* bindPlot, QMouseEvent* e);
    virtual bool handleMouseMove(QwtPlot* bindPlot, QMouseEvent* e);
    virtual bool handleMouseRelease(QwtPlot* bindPlot, QMouseEvent* e);
    virtual bool handleWheelEvent(QwtPlot* bindPlot, QWheelEvent* e);
    // Find the scale widget that should handle the event
    QwtScaleWidget* findTargetOnScale(const QPoint& pos);
};

#endif  // QWTPLOTSCALEEVENTDISPATCHER_H
