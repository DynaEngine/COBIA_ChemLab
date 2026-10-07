#ifndef QWT3D_COORDSYS_H
#define QWT3D_COORDSYS_H

#include "qwt3d_axis.h"
#include "qwt3d_colorlegend.h"

namespace Qwt3D
{

/**
 * @brief A coordinate system with different styles (BOX, FRAME)
 */
class QWT3D_EXPORT CoordinateSystem : public Drawable
{
    QWT_DECLARE_PRIVATE(CoordinateSystem)

public:
    explicit CoordinateSystem(Qwt3D::Triple blb = Qwt3D::Triple(0, 0, 0),
                              Qwt3D::Triple ftr = Qwt3D::Triple(0, 0, 0),
                              Qwt3D::COORDSTYLE = Qwt3D::BOX);
    ~CoordinateSystem() override;

    void init(Qwt3D::Triple beg = Qwt3D::Triple(0, 0, 0), Qwt3D::Triple end = Qwt3D::Triple(0, 0, 0));
    // Set style for the coordinate system (NOCOORD, FRAME or BOX)
    void setStyle(Qwt3D::COORDSTYLE s,
                  Qwt3D::AXIS frame_1 = Qwt3D::X1,
                  Qwt3D::AXIS frame_2 = Qwt3D::Y1,
                  Qwt3D::AXIS frame_3 = Qwt3D::Z1);
    // Return style of the coordinate system
    Qwt3D::COORDSTYLE style() const;
    // first == front_left_bottom, second == back_right_top
    void setPosition(Qwt3D::Triple first, Qwt3D::Triple second);

    // Set common color for all axes
    void setAxesColor(Qwt3D::RGBA val);
    // Set common font for all axis numberings
    void setNumberFont(QString const& family, int pointSize, int weight = QFont::Normal, bool italic = false);
    // Set common font for all axis numberings
    void setNumberFont(QFont const& font);
    // Set common color for all axis numberings
    void setNumberColor(Qwt3D::RGBA val);
    // Sets an linear axis with real number items
    void setStandardScale();

    // Fine tunes distance between axis numbering and axis body
    void adjustNumbers(int val);
    // Fine tunes distance between axis label and axis body
    void adjustLabels(int val);

    // Sets color for the grid lines
    void setGridLinesColor(Qwt3D::RGBA val);

    // Set common font for all axis labels
    void setLabelFont(QString const& family, int pointSize, int weight = QFont::Normal, bool italic = false);
    // Set common font for all axis labels
    void setLabelFont(QFont const& font);
    // Set common color for all axis labels
    void setLabelColor(Qwt3D::RGBA val);

    // Set line width for tic marks and axes
    void setLineWidth(double val, double majfac = 0.9, double minfac = 0.5);
    // Set length for tic marks
    void setTicLength(double major, double minor);

    // Switch autoscaling of axes
    void setAutoScale(bool val = true);

    Qwt3D::Triple first() const;
    Qwt3D::Triple second() const;

    void setAutoDecoration(bool val = true);
    bool autoDecoration() const;

    // Draw smooth axes
    void setLineSmooth(bool val = true);
    // Smooth axes enabled?
    bool lineSmooth() const;

    void draw() override;

    // Defines whether a grid between the major and/or minor tics should be drawn
    void setGridLines(bool majors, bool minors, int sides = Qwt3D::NOSIDEGRID);
    // Returns grids switched on
    int grids() const;

    // The vector of all 12 axes - use them to set axis properties individually
    std::vector< Axis > axes;

private:
    void destroy();
    void chooseAxes();
    void autoDecorateExposedAxis(Axis& ax, bool left);
    void drawMajorGridLines();
    void drawMinorGridLines();
    void drawMajorGridLines(Qwt3D::Axis&, Qwt3D::Axis&);
    void drawMinorGridLines(Qwt3D::Axis&, Qwt3D::Axis&);
    void recalculateAxesTics();
};

}  // ns

#endif
