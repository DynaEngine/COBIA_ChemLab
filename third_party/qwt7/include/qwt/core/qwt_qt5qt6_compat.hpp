#ifndef QWT_QT5QT6_COMPAT_HPP
#define QWT_QT5QT6_COMPAT_HPP
#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QFontMetricsF>

namespace qwt
{

/**
 * @brief Handle differences between Qt5 and Qt6
 *
 */
namespace compat
{

/**
 * @brief Get the event position (QPoint)
 * @tparam EventType Event type (must support pos() or position() method)
 * @param event Event pointer
 * @return Event position as QPoint
 */
template< typename EventType >
inline QPoint eventPos(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos();
#else
    return event->position().toPoint();
#endif
}

/**
 * @brief Get the x coordinate of the event
 * @tparam EventType Event type
 * @param event Event pointer
 * @return x coordinate (integer)
 */
template< typename EventType >
inline int eventPosX(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos().x();
#else
    return static_cast< int >(event->position().x());
#endif
}

/**
 * @brief Get the y coordinate of the event
 * @tparam EventType Event type
 * @param event Event pointer
 * @return y coordinate (integer)
 */
template< typename EventType >
inline int eventPosY(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos().y();
#else
    return static_cast< int >(event->position().y());
#endif
}

/**
 * @brief Calculate the horizontal advance of a string (integer version)
 * @param fm QFontMetrics object
 * @param str Target string
 * @return Width (integer)
 */
inline int horizontalAdvance(const QFontMetrics& fm, const QString& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    return fm.width(str);
#else
    return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief Calculate the horizontal advance of a string (floating-point version)
 * @param fm QFontMetricsF object
 * @param str Target string
 * @return Width (floating-point)
 */
inline qreal horizontalAdvanceF(const QFontMetricsF& fm, const QString& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    return fm.width(str);
#else
    return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief Get vertical wheel delta value compatible with Qt5 and Qt6
 *
 * This function provides a unified interface to retrieve the vertical scroll delta
 * from a QWheelEvent, supporting both Qt5 (using delta()) and Qt6 (using angleDelta().y())
 * without changing the calling code.
 *
 * The return value represents the vertical scroll amount:
 * - Positive value: Wheel scrolled up
 * - Negative value: Wheel scrolled down
 * - The magnitude follows the standard wheel step (typically ±120 per notch)
 *
 * @param e Pointer to the QWheelEvent object (must not be nullptr)
 * @return Integer delta value of vertical wheel movement
 * @note The function only returns vertical wheel delta (ignores horizontal scroll via angleDelta().x())
 * @warning Ensure the input QWheelEvent pointer is valid to avoid null pointer dereference
 */
inline int wheelEventDelta(QWheelEvent* e)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return e->delta();
#else
    return e->angleDelta().y();
#endif
}
}  // namespace   compat
}  // namespace   qwt
#endif  // QWT_QT5QT6_COMPAT_HPP
