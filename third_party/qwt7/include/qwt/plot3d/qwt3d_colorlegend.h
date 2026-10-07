#ifndef QWT3D_COLORLEGEND_H
#define QWT3D_COLORLEGEND_H

#include "qwt3d_global.h"
#include "qwt3d_drawable.h"
#include "qwt3d_axis.h"
#include "qwt3d_color.h"

namespace Qwt3D
{

/**
 * @brief A flat color legend
 * @details The class visualizes a ColorVector together with a scale (axis)
 *          and a caption. ColorLegends are vertical or horizontal.
 */
class QWT3D_EXPORT ColorLegend : public Drawable
{
    QWT_DECLARE_PRIVATE(ColorLegend)

public:
    // Possible anchor points for caption and axis
    enum SCALEPOSITION
    {
        Top,
        Bottom,
        Left,
        Right
    };

    // Orientation of the legend
    enum ORIENTATION
    {
        BottomTop,
        LeftRight
    };

    // Standard constructor
    ColorLegend();
    ~ColorLegend() override;

    // Draws the object - called by updateGL()
    virtual void draw() override;

    // Sets the relative position of the legend inside widget
    void setRelPosition(Qwt3D::Tuple relMin, Qwt3D::Tuple relMax);
    // Sets legend orientation and scale position
    void setOrientation(ORIENTATION, SCALEPOSITION);
    // Sets the limit of the scale
    void setLimits(double start, double stop);
    // Sets scale major tics
    void setMajors(int);
    // Sets scale minor tics
    void setMinors(int);
    // Sets whether a scale will be drawn
    void drawScale(bool val);
    // Sets whether the scale will have scale numbers
    void drawNumbers(bool val);
    // Sets whether the axis is autoscaled or not
    void setAutoScale(bool val);
    // Sets another scale
    void setScale(Qwt3D::Scale* scale);
    // Sets one of the predefined scale types
    void setScale(Qwt3D::SCALETYPE);

    // Sets the legends caption string
    void setTitleString(QString const& s);

    // Sets the legends caption font
    void setTitleFont(QString const& family, int pointSize, int weight = QFont::Normal, bool italic = false);

    // The color vector
    Qwt3D::ColorVector colors;

private:
    Qwt3D::ParallelEpiped geometry() const;
    void setGeometryInternal();
};

}  // ns

#endif
