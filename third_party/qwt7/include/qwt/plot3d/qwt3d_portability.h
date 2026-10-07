#ifndef QWT3D_PORTABILITY_H
#define QWT3D_PORTABILITY_H

#include <qnamespace.h>
#include "qwt3d_global.h"

#include <QMouseEvent>

namespace Qwt3D
{

#define QWT3DLOCAL8BIT(qstring) (qstring.toLocal8Bit().constData())

const Qt::TextFlag SingleLine = Qt::TextSingleLine;

/**
 * @brief Creates a (mouse-button, modifier) pair
 * @details This class encapsulates a combination of mouse buttons and keyboard modifiers,
 *          used for defining mouse interaction states in 3D plots.
 */
class MouseState
{
public:
    MouseState(Qt::MouseButtons mb = Qt::NoButton, Qt::KeyboardModifiers km = Qt::NoModifier) : mb_(mb), km_(km)
    {
    }

    MouseState(Qt::MouseButton mb, Qt::KeyboardModifiers km = Qt::NoModifier) : mb_(mb), km_(km)
    {
    }

    bool operator==(const MouseState& ms)
    {
        return mb_ == ms.mb_ && km_ == ms.km_;
    }

    bool operator!=(const MouseState& ms)
    {
        return !operator==(ms);
    }

private:
    Qt::MouseButtons mb_;
    Qt::KeyboardModifiers km_;
};

/**
 * @brief Creates a (key-button, modifier) pair
 * @details This class encapsulates a combination of keyboard keys and modifiers,
 *          used for defining keyboard interaction states in 3D plots.
 */
class KeyboardState
{
public:
    KeyboardState(int key = Qt::Key_unknown, Qt::KeyboardModifiers km = Qt::NoModifier) : key_(key), km_(km)
    {
    }

    bool operator==(const KeyboardState& ms)
    {
        return key_ == ms.key_ && km_ == ms.km_;
    }

    bool operator!=(const KeyboardState& ms)
    {
        return !operator==(ms);
    }

private:
    int key_;
    Qt::KeyboardModifiers km_;
};
}  // ns

#endif  // QWT3D_PORTABILITY_H