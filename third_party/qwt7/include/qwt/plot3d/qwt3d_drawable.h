#ifndef QWT3D_DRAWABLE_H
#define QWT3D_DRAWABLE_H

#include "qwt3d_global.h"
#include "qwt3d_types.h"
#include "qwt3d_io_gl2ps.h"

namespace Qwt3D
{

/**
 * @brief Abstract base class for Drawables
 */
class QWT3D_EXPORT Drawable
{
    QWT_DECLARE_PRIVATE(Drawable)

public:
    virtual ~Drawable() = 0;

    virtual void draw();

    virtual void saveGLState();
    virtual void restoreGLState();

    void attach(Drawable*);
    void detach(Drawable*);
    void detachAll();

    virtual void setColor(double r, double g, double b, double a = 1);
    virtual void setColor(Qwt3D::RGBA rgba);
    Qwt3D::Triple relativePosition(Qwt3D::Triple rel);

protected:
    Qwt3D::RGBA color;
    void Enable(GLenum what, GLboolean val);
    Qwt3D::Triple ViewPort2World(Qwt3D::Triple win, bool* err = nullptr);
    Qwt3D::Triple World2ViewPort(Qwt3D::Triple obj, bool* err = nullptr);

    Drawable();
    Drawable(Drawable&& other) noexcept;
    Drawable& operator=(Drawable&& other) noexcept;

    GLdouble modelMatrix[ 16 ];
    GLdouble projMatrix[ 16 ];
    GLint viewport[ 4 ];
};

}  // ns

#endif