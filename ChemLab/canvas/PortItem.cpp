#include "PortItem.h"
#include "UnitOperationItem.h"
#include "FlowsheetScene.h"

#include <QPen>
#include <QBrush>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

PortItem::PortItem(const QString& name, Direction dir, UnitOperationItem* parent)
	: QGraphicsEllipseItem(-6, -6, 12, 12, parent)
	, m_name(name)
	, m_dir(dir)
	, m_unit(parent)
{
	setAcceptHoverEvents(true);
	setCursor(Qt::CrossCursor);
	setZValue(1);

	QColor color = (dir == Direction::Input) ? QColor(0x4a, 0x9e, 0xff) : QColor(0x00, 0xc8, 0x53);
	setPen(QPen(color.darker(130), 1.5));
	setBrush(color);
	setToolTip(name);
}

qreal PortItem::loc() const
{
	if (!m_unit) return 0.5;
	qreal h = m_unit->rect().height();
	return (h > 0.0) ? (pos().y() / h) : 0.5;
}

QPointF PortItem::centerInScene() const
{
	return mapToScene(QPointF(0, 0));
}

void PortItem::setHighlighted(bool on)
{
	if (on)
	{
		setScale(1.5);
		setPen(QPen(QColor(0xff, 0xd7, 0x00), 2.5));
	}
	else
	{
		setScale(1.0);
		QColor color = (m_dir == Direction::Input) ? QColor(0x4a, 0x9e, 0xff) : QColor(0x00, 0xc8, 0x53);
		setPen(QPen(color.darker(130), 1.5));
	}
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	m_hovered = true;
	setHighlighted(true);
	QGraphicsEllipseItem::hoverEnterEvent(event);
}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	m_hovered = false;
	setHighlighted(false);
	QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	event->accept();
}

void PortItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_dir != Direction::Output)
	{
		event->ignore();
		return;
	}

	auto* fsScene = qobject_cast<FlowsheetScene*>(scene());
	if (fsScene)
	{
		fsScene->startTemporaryConnection(this);
		event->accept();
		return;
	}
	event->ignore();
}