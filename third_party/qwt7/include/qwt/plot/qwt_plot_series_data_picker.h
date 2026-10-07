/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 2024   ChenZongYan <czy.t@163.com>
 *****************************************************************************/
#ifndef QWT_PLOT_SERIES_DATA_PICKER_H
#define QWT_PLOT_SERIES_DATA_PICKER_H
#include <QList>
#include <QPointF>
#include <QVariant>
#include "qwt_canvas_picker.h"
#include "qwt_text.h"
class QwtPlot;
class QwtPlotItem;

/**
 * @brief A plot data picker class for displaying current y-values or nearest points
 * @details QwtPlotSeriesDataPicker is a plot data picker class that displays current y-values
 *          or the nearest points to the mouse cursor position.
 */
class QWT_EXPORT QwtPlotSeriesDataPicker : public QwtCanvasPicker
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtPlotSeriesDataPicker)
public:
    /**
     * @brief Pick modes
     */
    enum PickSeriesMode
    {
        PickYValue,       ///< Pick y-value (default)
        PickNearestPoint  ///< Pick the nearest point to the mouse cursor position (this mode may be time-consuming, use with caution when there are many curve points)
    };

    /**
     * @brief Text placement options
     */
    enum TextPlacement
    {
        TextPlaceAuto,            ///< Auto placement (top for pick y, follow mouse for pick nearest)
        TextFollowOnTop,          ///< On top of the plot area (default)
        TextFollowOnBottom,       ///< On bottom of the plot area
        TextFollowMouse,          ///< Follow mouse pointer
        TextOnCanvasTopRight,     ///< Text on canvas top right
        TextOnCanvasTopLeft,      ///< Text on canvas top left
        TextOnCanvasBottomRight,  ///< Text on canvas bottom right
        TextOnCanvasBottomLeft,   ///< Text on canvas bottom left
        TextOnCanvasTopAuto,      ///< Text on canvas top, left or right auto-detected based on mouse position
        TextOnCanvasBottomAuto    ///< Text on canvas bottom, left or right auto-detected based on mouse position
    };

    /**
     * @brief Interpolation modes
     */
    enum InterpolationMode
    {
        NoInterpolation,     ///< No interpolation, use nearest data point
        LinearInterpolation  ///< Linear interpolation between adjacent data points
    };

    /**
     * @brief Feature point structure
     */
    struct FeaturePoint
    {
        QwtPlotItem* item { nullptr };  ///< Corresponding item
        QPointF feature { 0, 0 };       ///< Feature point position (for screen drawing)
        QVariant sampleData;            ///< Full sample data (QwtOHLCSample, QwtIntervalSample, etc.)
        size_t index { 0 };             ///< Index in the item
    };

public:
    /// Constructor
    explicit QwtPlotSeriesDataPicker(QWidget* canvas);
    /// Destructor
    ~QwtPlotSeriesDataPicker() override;

    /// Set pick mode
    void setPickMode(PickSeriesMode mode);
    /// Get pick mode
    PickSeriesMode pickMode() const;

    /// Set text placement
    void setTextArea(TextPlacement t);
    /// Get text placement
    TextPlacement textArea() const;

    /// Set interpolation mode
    void setInterpolationMode(InterpolationMode mode);
    /// Get interpolation mode
    InterpolationMode interpolationMode() const;
    /// Check if interpolation is enabled
    bool isInterpolation() const;

    /// Set nearest search window size
    void setNearestSearchWindowSize(int windowSize);
    /// Get nearest search window size
    int nearestSearchWindowSize() const;

    /// Enable/disable feature point drawing
    void setEnableDrawFeaturePoint(bool on = true);
    /// Check if feature point drawing is enabled
    bool isEnableDrawFeaturePoint() const;

    /// Set feature point size
    void setDrawFeaturePointSize(int px);
    /// Get feature point size
    int drawFeaturePointSize() const;

    // Returns the list of feature points currently picked by the tracker
    QList< FeaturePoint > featurePoints() const;

    /// Set text background brush
    void setTextBackgroundBrush(const QBrush& br);
    /// Get text background brush
    QBrush textBackgroundBrush() const;

    /// Set text alignment
    void setTextAlignment(Qt::Alignment al);
    /// Get text alignment
    Qt::Alignment textAlignment() const;

    /// Whether to show X value
    void setEnableShowXValue(bool on);
    bool isEnableShowXValue() const;

    /// Set the tracker rectangle offset in TextFollowMouse mode
    void setTextTrackerOffset(const QPoint& offset);
    QPoint textTrackerOffset() const;

    /// Top rectangle text
    QwtText trackerText(const QPoint& pos) const override;

    /// Keep rectangle at top
    QRect trackerRect(const QFont& f) const override;

    /// Draw rubber band
    virtual void drawRubberBand(QPainter* painter) const override;

    /// Manually set position
    virtual void setTrackerPosition(const QPoint& pos) override;

protected:
    /// Get all pickable Y values at the specified screen position; returns the number picked
    virtual int pickYValue(const QwtPlot* p, const QPoint& pos, bool interpolate = false);
    /// Get the nearest pickable point at the specified screen position (window-based fast indexing)
    virtual int pickNearestPoint(const QwtPlot* plot, const QPoint& pos, int windowSize = -5);

    virtual void widgetMousePressEvent(QMouseEvent* event) override;
    virtual void widgetMouseDoubleClickEvent(QMouseEvent* event) override;
Q_SIGNALS:
    /**
     * @brief Emitted when the user left-clicks on the plot canvas
     * @param picker Pointer to the picker that was clicked
     * @param pos Screen position of the click event
     * @note A double-click will trigger clicked() before doubleClicked().
     *       Connect only one of these signals if you need to distinguish
     *       single-click from double-click.
     */
    void clicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);

    /**
     * @brief Emitted when the user double-left-clicks on the plot canvas
     * @param picker Pointer to the picker that was double-clicked
     * @param pos Screen position of the double-click event
     * @note A double-click also triggers clicked() before this signal.
     */
    void doubleClicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);

private Q_SLOTS:
    // Slot for item detachment, used to update records
    void onPlotItemDetached(QwtPlotItem* item, bool on);
    void onParasitePlotAttached(QwtPlot* parasiteplot, bool on);

protected:
    // Generate text content for an item
    virtual QString valueString(const QList< FeaturePoint >& fps) const;
    // Draw a feature point
    virtual void
    drawFeaturePoint(QPainter* painter, const QwtPlot* plot, const QwtPlotItem* item, const QPointF& itemPoint) const;
    // Mouse move
    virtual void move(const QPoint& pos) override;
    // Format value according to axis type; for date axes the value is a large float but users need to see a format like 2024-10-01
    QString formatAxisValue(double value, int axisId, QwtPlot* plot) const;

private:
    // Draw feature points (captured points)
    void drawAllFeaturePoints(QPainter* painter) const;
    // Update feature points
    void updateFeaturePoint(const QPoint& pos);
    //
    QRect ensureRectInBounds(const QRect& rect, const QRect& bounds) const;
};

#endif  // QWT_PLOT_SERIES_DATA_PICKER_H
