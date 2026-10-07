#ifndef QWT_FIGURE_H
#define QWT_FIGURE_H
// stl
#include <memory>

// Qt
#include <QFrame>
class QResizeEvent;
class QPaintEvent;

// qwt
#include "qwt_global.h"
#include "qwt_axis.h"
class QwtPlot;

/**
 * @class QwtFigure
 *
 * @brief A figure container for organizing Qwt plots with flexible layout options
 *
 * @details
 * This class provides a figure-like container similar to matplotlib's Figure class,
 * supporting both normalized coordinate positioning and grid layouts for Qwt plots.
 * It uses Qt's standard top-left coordinate system for intuitive positioning.
 *
 * @par Example:
 * @code
 * // Example usage:
 * QwtFigure figure;
 *
 * // Add a plot using normalized coordinates (Qt top-left coordinate system)
 * QwtPlot* plot1 = new QwtPlot;
 * figure.addAxes(plot1, QRectF(0.1, 0.1, 0.8, 0.4));
 *
 * // Add plots using grid layout - Create a 2x2 grid
 * QwtPlot* plot2 = new QwtPlot;
 * figure.addAxes(plot2, 2, 2, 0, 1); // 2x2 grid, row 0, column 1
 *
 * QwtPlot* plot3 = new QwtPlot;
 * figure.addAxes(plot3, 2, 2, 1, 0, 1, 2); // row 1, columns 0-1 (span 2 columns)
 *
 * // Save the figure
 * figure.saveFig("output.png", 300);
 * @endcode
 */
class QWT_EXPORT QwtFigure : public QFrame
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtFigure)
public:
    QwtFigure(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QwtFigure() override;

    // Add a widget with normalized coordinates
    void addWidget(QWidget* widget, qreal left, qreal top, qreal width, qreal height);
    void addWidget(QWidget* widget,
                   int rowCnt,
                   int colCnt,
                   int row,
                   int col,
                   int rowSpan  = 1,
                   int colSpan  = 1,
                   qreal wspace = 0.0,
                   qreal hspace = 0.0);

    // Add a plot with normalized coordinates
    void addAxes(QwtPlot* plot, const QRectF& rect);

    // Add a plot with normalized coordinates using separate parameters
    void addAxes(QwtPlot* plot, qreal left, qreal top, qreal width, qreal height);

    // Add a plot by grid layout
    void addGridAxes(QwtPlot* plot,
                     int rowCnt,
                     int colCnt,
                     int row,
                     int col,
                     int rowSpan  = 1,
                     int colSpan  = 1,
                     qreal wspace = 0.0,
                     qreal hspace = 0.0);

    // Change the normalized position of an added widget
    void setWidgetNormPos(QWidget* widget, const QRectF& rect);

    // Get all axes (plots) in the figure (not including parasite axes)
    QList< QwtPlot* > allAxes(bool byZOrder = false) const;

    // Check if the figure has any axes
    bool hasAxes() const;

    // Check if the figure has a specific plot
    bool hasAxes(QwtPlot* plot) const;

    // Remove a specific axes (plot) from the figure
    void removeAxes(QwtPlot* plot);

    // Take a specific axes (plot) from the figure without deleting it
    bool takeAxes(QwtPlot* plot);

    // Clear all axes from the figure
    void clear();

    // Get the size of the figure in inches
    QSize getSizeInches() const;

    // Set the size of the figure in inches
    void setSizeInches(float width, float height);
    void setSizeInches(const QSizeF& size);

    // Set the face color of the figure
    void setFaceColor(const QColor& color);
    QColor faceColor() const;

    // Set the face brush of the figure
    void setFaceBrush(const QBrush& brush);
    QBrush faceBrush() const;

    // Set the edge color of the figure
    void setEdgeColor(const QColor& color);
    QColor edgeColor() const;

    // Set the edge line width of the figure
    void setEdgeLineWidth(int width);
    int edgeLineWidth() const;

    // Create parasite axes for a host plot
    QwtPlot* createParasiteAxes(QwtPlot* hostPlot, QwtAxis::Position enableAxis);

    // Get all parasite axes for a host plot
    QList< QwtPlot* > getParasiteAxes(QwtPlot* hostPlot) const;

    // Save the figure to a QPixmap with specified DPI
    QPixmap saveFig(int dpi = -1) const;

    // Save the figure to a QPixmap with specified size in inches
    QPixmap saveFig(QSizeF& inchesSize) const;

    // Save the figure to a file with specified DPI
    bool saveFig(const QString& filename, int dpi = -1) const;

    // Set the current axes (plot)
    void setCurrentAxes(QwtPlot* plot);
    void sca(QwtPlot* plot);

    // Get the current axes (plot)
    QwtPlot* currentAxes() const;
    QwtPlot* gca() const;

    // Get the normalized rectangle for an axes
    QRectF axesNormRect(QwtPlot* plot) const;

    // Get the normalized rectangle for a child widget
    QRectF widgetNormRect(QWidget* w) const;

    // Get the plot at a given position (returns host plot if parasite exists)
    QwtPlot* plotUnderPos(const QPoint& pos) const;

    // Calculate normalized coordinates from actual geometry
    QRectF calcNormRect(const QRect& geoRect) const;

    // Calculate actual rectangle from normalized coordinates
    QRect calcActualRect(const QRectF& normRect);

    // Update all plots
    void replotAll();

    // Add axis alignment configuration
    void addAxisAlignment(const QList< QwtPlot* >& plots, int axisId);

    // Remove specified axis alignment configuration
    bool removeAxisAlignment(const QList< QwtPlot* >& plots, int axisId);

    // Clear all axis alignment configurations
    void clearAxisAlignment();

    // Apply all axis alignment configurations
    void applyAllAxisAlignments(bool replot = true);

    // Apply all alignment configurations for a specific axis ID
    void applyAlignmentsForAxis(int axisId);

    // Get the count of axis alignment configurations
    int axisAligmentCount() const;

    // Get axis alignment information at specified index
    QPair< QList< QwtPlot* >, int > axisAligmentInfo(int index) const;

public:
    // Align axes of multiple QwtPlot objects
    static void alignAxes(QList< QwtPlot* > plots, int axisId, bool update = true);
Q_SIGNALS:
    /**
     * @brief Signal emitted when axes are added to the figure
     * @param newAxes Pointer to the newly added QwtPlot
     * @note Parasite axes addition also triggers this signal
     */
    void axesAdded(QwtPlot* newAxes);

    /**
     * @brief Signal emitted when axes are removed from the figure
     * @param removedAxes Pointer to the removed QwtPlot
     * @note Parasite axes removal also triggers this signal
     */
    void axesRemoved(QwtPlot* removedAxes);

    /**
     * @brief Signal emitted when the figure is cleared
     */
    void figureCleared();

    /**
     * @brief Signal emitted when the current active axes changes
     * @param current Pointer to the current QwtPlot, nullptr if no valid axes
     * @note Parasite axes cannot be set as current axes
     */
    void currentAxesChanged(QwtPlot* current);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};

#endif  // QWT_FIGURE_H
