#ifndef QWT3D_PLOT_H
#define QWT3D_PLOT_H

#include <QOpenGLWidget>

#include "qwt3d_coordsys.h"
#include "qwt3d_enrichment_std.h"
#include "qwt3d_theme.h"

namespace Qwt3D
{

/**
 * @brief Base class for all plotting widgets
 * @details Plot3D handles all the common features for plotting widgets -
 *          coordinate system, transformations, mouse/keyboard handling,
 *          labeling etc. It contains some pure virtual functions and is,
 *          in so far, an abstract base class. The class provides interfaces
 *          for data handling and implements basic data controlled color allocation.
 */
class QWT3D_EXPORT Plot3D : public QOpenGLWidget
{
    Q_OBJECT

public:
    // Constructor
    Plot3D(QWidget* parent = nullptr);
    // Destructor
    ~Plot3D() override;

    // Render to pixmap
    QPixmap renderPixmap(int w = 0, int h = 0, bool useContext = false);
    // Recalculate data
    void updateData();
    // Create coordinate system between two points
    void createCoordinateSystem(Qwt3D::Triple beg, Qwt3D::Triple end);
    // Returns pointer to CoordinateSystem object
    Qwt3D::CoordinateSystem* coordinates();
    // Returns pointer to ColorLegend object
    Qwt3D::ColorLegend* legend();

    // Returns rotation around X axis [-360..360] (some angles are equivalent)
    double xRotation() const;
    // Returns rotation around Y axis [-360..360] (some angles are equivalent)
    double yRotation() const;
    // Returns rotation around Z axis [-360..360] (some angles are equivalent)
    double zRotation() const;

    // Returns shift along X axis (object coordinates)
    double xShift() const;
    // Returns shift along Y axis (object coordinates)
    double yShift() const;
    // Returns shift along Z axis (object coordinates)
    double zShift() const;

    // Returns relative shift [-1..1] along X axis (view coordinates)
    double xViewportShift() const;
    // Returns relative shift [-1..1] along Y axis (view coordinates)
    double yViewportShift() const;

    // Returns scaling for X values [0..inf]
    double xScale() const;
    // Returns scaling for Y values [0..inf]
    double yScale() const;
    // Returns scaling for Z values [0..inf]
    double zScale() const;

    // Returns zoom (0..inf)
    double zoom() const;

    // Returns orthogonal (true) or perspective (false) projection
    bool ortho() const;
    // Set plot style
    void setPlotStyle(Qwt3D::PLOTSTYLE val);
    // Set plot style with Enrichment
    Qwt3D::Enrichment* setPlotStyle(Qwt3D::Enrichment const& val);
    // Returns plotting style
    Qwt3D::PLOTSTYLE plotStyle() const;
    // Returns current Enrichment object used for plotting styles (if set, zero else)
    Qwt3D::Enrichment* userStyle() const;
    // Set shading style
    void setShading(Qwt3D::SHADINGSTYLE val);
    // Returns shading style
    Qwt3D::SHADINGSTYLE shading() const;
    // Set number of isolines
    void setIsolines(int isolines);
    // Returns number of isolines
    int isolines() const;

    // Enables/disables smooth data mesh lines. Default is false
    void setSmoothMesh(bool val);
    // True if mesh antialiasing is on
    bool smoothDataMesh() const;
    // Sets widgets background color
    void setBackgroundColor(Qwt3D::RGBA rgba);
    // Returns the widgets background color
    Qwt3D::RGBA backgroundRGBAColor() const;
    // Sets color for data mesh
    void setMeshColor(Qwt3D::RGBA rgba);
    // Returns color for data mesh
    Qwt3D::RGBA meshColor() const;
    // Sets line width for data mesh
    void setMeshLineWidth(double lw);
    // Returns line width for data mesh
    double meshLineWidth() const;
    // Sets new data color object
    void setDataColor(Color* col);
    // Returns data color object
    const Color* dataColor() const;

    // Add an Enrichment
    virtual Qwt3D::Enrichment* addEnrichment(Qwt3D::Enrichment const&);
    // Remove an Enrichment
    virtual bool degrade(Qwt3D::Enrichment*);

    // Returns rectangular hull
    Qwt3D::ParallelEpiped hull() const;

    // Show/hide color legend
    void showColorLegend(bool);

    // Sets style of coordinate system
    void setCoordinateStyle(Qwt3D::COORDSTYLE st);
    // Set polygon offset
    void setPolygonOffset(double d);
    // Returns relative value for polygon offset [0..1]
    double polygonOffset() const;

    // Set title position
    void setTitlePosition(double rely, double relx = 0.5, Qwt3D::ANCHOR = Qwt3D::TopCenter);
    // Set title font
    void setTitleFont(const QString& family, int pointSize, int weight = QFont::Normal, bool italic = false);
    // Set caption color
    void setTitleColor(Qwt3D::RGBA col);
    // Set caption text (one row only)
    void setTitle(const QString& title);

    // Set and apply a theme
    void setTheme(const Qwt3DTheme& theme);
    // Get the current theme
    Qwt3DTheme theme() const;

    // Apply a theme preset by enum
    void applyTheme(Qwt3DTheme::Preset preset);
    // Apply a theme preset by name
    void applyTheme(const QString& presetName);

    // Assign mouse states for rotations, scales, zoom and shifts
    void assignMouse(MouseState xrot,
                     MouseState yrot,
                     MouseState zrot,
                     MouseState xscale,
                     MouseState yscale,
                     MouseState zscale,
                     MouseState zoom,
                     MouseState xshift,
                     MouseState yshift);

    // Returns true, if the widget accept mouse input from the user
    bool mouseEnabled() const;
    // Assign keyboard states for rotations, scales, zoom and shifts
    void assignKeyboard(KeyboardState xrot_n,
                        KeyboardState xrot_p,
                        KeyboardState yrot_n,
                        KeyboardState yrot_p,
                        KeyboardState zrot_n,
                        KeyboardState zrot_p,
                        KeyboardState xscale_n,
                        KeyboardState xscale_p,
                        KeyboardState yscale_n,
                        KeyboardState yscale_p,
                        KeyboardState zscale_n,
                        KeyboardState zscale_p,
                        KeyboardState zoom_n,
                        KeyboardState zoom_p,
                        KeyboardState xshift_n,
                        KeyboardState xshift_p,
                        KeyboardState yshift_n,
                        KeyboardState yshift_p);

    // Returns true, if the widget accept keyboard input from the user
    bool keyboardEnabled() const;
    // Sets speed for keyboard driven transformations
    void setKeySpeed(double rot, double scale, double shift);
    // Gets speed for keyboard driven transformations
    void keySpeed(double& rot, double& scale, double& shift) const;

    // Returns true, if Lighting is enabled, false else
    bool lightingEnabled() const;
    // Turn light on
    void illuminate(unsigned light = 0);
    // Turn light off
    void blowout(unsigned light = 0);

    // Set material component (RGBA)
    void setMaterialComponent(GLenum property, double r, double g, double b, double a = 1.0);
    // Set material component (intensity)
    void setMaterialComponent(GLenum property, double intensity);
    // Set shininess exponent
    void setShininess(double exponent);
    // Set light component (RGBA)
    void setLightComponent(GLenum property, double r, double g, double b, double a = 1.0, unsigned light = 0);
    // Set light component (intensity)
    void setLightComponent(GLenum property, double intensity, unsigned light = 0);

    // Returns Light 'idx' rotation around X axis [-360..360] (some angles are equivalent)
    double xLightRotation(unsigned idx = 0) const;
    // Returns Light 'idx' rotation around Y axis [-360..360] (some angles are equivalent)
    double yLightRotation(unsigned idx = 0) const;
    // Returns Light 'idx' rotation around Z axis [-360..360] (some angles are equivalent)
    double zLightRotation(unsigned idx = 0) const;

    // Returns shift of Light 'idx' along X axis (object coordinates)
    double xLightShift(unsigned idx = 0) const;
    // Returns shift of Light 'idx' along Y axis (object coordinates)
    double yLightShift(unsigned idx = 0) const;
    // Returns shift of Light 'idx' along Z axis (object coordinates)
    double zLightShift(unsigned idx = 0) const;
    // Returns true if valid data available, false else
    bool hasData() const;

Q_SIGNALS:

    /**
     * @brief Signal emitted when the rotation is changed
     * @param xAngle X axis rotation angle
     * @param yAngle Y axis rotation angle
     * @param zAngle Z axis rotation angle
     */
    void rotationChanged(double xAngle, double yAngle, double zAngle);

    /**
     * @brief Signal emitted when the shift is changed
     * @param xShift X axis shift value
     * @param yShift Y axis shift value
     * @param zShift Z axis shift value
     */
    void shiftChanged(double xShift, double yShift, double zShift);

    /**
     * @brief Signal emitted when the viewport shift is changed
     * @param xShift X viewport shift value
     * @param yShift Y viewport shift value
     */
    void vieportShiftChanged(double xShift, double yShift);

    /**
     * @brief Signal emitted when the scaling is changed
     * @param xScale X axis scale factor
     * @param yScale Y axis scale factor
     * @param zScale Z axis scale factor
     */
    void scaleChanged(double xScale, double yScale, double zScale);

    /**
     * @brief Signal emitted when the zoom is changed
     * @param zoom Zoom factor
     */
    void zoomChanged(double);

    /**
     * @brief Signal emitted when the projection mode is changed
     * @param ortho True for orthogonal, false for perspective
     */
    void projectionChanged(bool);

public Q_SLOTS:

    // Set rotation values
    void setRotation(double xVal, double yVal, double zVal);
    // Set shift values
    void setShift(double xVal, double yVal, double zVal);
    // Set viewport shift values
    void setViewportShift(double xVal, double yVal);
    // Set scale values
    void setScale(double xVal, double yVal, double zVal);
    // Set zoom value
    void setZoom(double);

    // Set orthogonal/perspective projection
    void setOrtho(bool);

    // Enable mouse input
    void enableMouse(bool val = true);
    // Disable mouse input
    void disableMouse(bool val = true);
    // Enable keyboard input
    void enableKeyboard(bool val = true);
    // Disable keyboard input
    void disableKeyboard(bool val = true);

    // Turn Lighting on or off
    void enableLighting(bool val = true);
    // Turn Lighting on or off
    void disableLighting(bool val = true);

    // Set light rotation
    void setLightRotation(double xVal, double yVal, double zVal, unsigned int idx = 0);
    // Set light shift
    void setLightShift(double xVal, double yVal, double zVal, unsigned int idx = 0);

    // Saves content to pixmap format
    virtual bool savePixmap(QString const& fileName, QString const& format);
    // Saves content to vector format
    virtual bool saveVector(QString const& fileName,
                            QString const& format,
                            VectorWriter::TEXTMODE text,
                            VectorWriter::SORTMODE sortmode);
    // Saves content
    virtual bool save(QString const& fileName, QString const& format);

protected:
    QWT_DECLARE_PRIVATE(Plot3D)

    using EnrichmentList = std::list< Qwt3D::Enrichment* >;
    using ELIT           = EnrichmentList::iterator;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    void keyPressEvent(QKeyEvent* e) override;

    // Protected accessors for derived classes
    std::vector< GLuint >& displayLists();
    Qwt3D::Data* actualData() const;
    void setActualData(Qwt3D::Data* data);

    virtual void calculateHull() = 0;
    virtual void createData()    = 0;
    virtual void createEnrichment(Qwt3D::Enrichment&)
    {
    }
    virtual void createEnrichments();

    void createCoordinateSystem();
    void setHull(Qwt3D::ParallelEpiped p);

    bool initializedGL() const;

    enum OBJECTS
    {
        DataObject,
        LegendObject,
        NormalObject,
        DisplayListSize  // only to have a vector length ...
    };

private:
    void setRotationMouse(MouseState bstate, double accel, QPoint diff);
    void setScaleMouse(MouseState bstate, double accel, QPoint diff);
    void setShiftMouse(MouseState bstate, double accel, QPoint diff);

    void setRotationKeyboard(KeyboardState kseq, double speed);
    void setScaleKeyboard(KeyboardState kseq, double speed);
    void setShiftKeyboard(KeyboardState kseq, double speed);

    void applyLight(unsigned idx);
    void applyLights();
};

}  // ns

#endif  // QWT3D_PLOT_H
