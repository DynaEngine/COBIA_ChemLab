#ifndef QWT_PLOT_SERIES_DATA_PICKER_GROUP_H
#define QWT_PLOT_SERIES_DATA_PICKER_GROUP_H
#include <QObject>
#include "qwt_global.h"
class QwtPlotSeriesDataPicker;
/**
 * @brief Group manager for synchronizing multiple QwtPlotSeriesDataPicker instances across plot widgets
 *
 * This class enables coordinated interaction among multiple plot pickers, currently implementing
 * synchronized X-axis proportional positioning. When a user moves the mouse over one plot within
 * a subplot arrangement, the active picker displays its Y-value while all other pickers in the same
 * group automatically position themselves at equivalent X-axis proportional locations on their
 * respective plots, creating a unified cross-plot data inspection experience.
 *
 * Key features:
 * - Automatic synchronization of picker positions based on X-axis proportion
 * - Dynamic membership management (add/remove pickers at runtime)
 * - Automatic cleanup when pickers are destroyed
 * - Activation state synchronization across the group
 *
 * @note Currently supports only X-axis proportional synchronization; Y-axis values are displayed
 *       independently per plot based on local data at the synchronized X position.
 */
class QWT_EXPORT QwtPlotSeriesDataPickerGroup : public QObject
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtPlotSeriesDataPickerGroup)
public:
    /// Constructor
    QwtPlotSeriesDataPickerGroup(QObject* par = nullptr);
    /// Destructor
    ~QwtPlotSeriesDataPickerGroup() override;
    /// Add a picker to the group
    void addPicker(QwtPlotSeriesDataPicker* pick);
    /// Remove a picker from the group
    void removePicker(QwtPlotSeriesDataPicker* pick);
    /// Get all pickers in the group
    QList< QwtPlotSeriesDataPicker* > pickers() const;
    /// Set enabled state
    void setEnabled(bool on);
    /// Check if enabled
    bool isEnabled() const;

Q_SIGNALS:
    /**
     * @brief Emitted when a picker in the group is clicked
     * @param picker The picker that was clicked
     * @param pos The click position in picker's canvas coordinates
     */
    void clicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);

    /**
     * @brief Emitted when a picker in the group is double-clicked
     * @param picker The picker that was double-clicked
     * @param pos The double-click position in picker's canvas coordinates
     */
    void doubleClicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);

public Q_SLOTS:
    void onPickerMove(const QPoint& pos);
    void onPickerActivated(bool on);
    void onPickerDestroy(QObject* obj);
    void onPickerClicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);
    void onPickerDoubleClicked(QwtPlotSeriesDataPicker* picker, const QPoint& pos);
};

#endif  // QWT_PLOT_SERIES_DATA_PICKER_GROUP_H
