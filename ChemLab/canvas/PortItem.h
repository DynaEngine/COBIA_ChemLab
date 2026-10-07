#pragma once

#include <QGraphicsEllipseItem>
#include <QString>
#include <QPen>
#include <QChar>

class UnitOperationItem;

class PortItem : public QGraphicsEllipseItem
{
public:
	enum class Direction { Input, Output };

	PortItem(const QString& name, Direction dir, UnitOperationItem* parent);

	QString portName() const { return m_name; }
	Direction direction() const { return m_dir; }
	UnitOperationItem* unitItem() const { return m_unit; }

	QChar side() const { return (m_dir == Direction::Output) ? QChar('E') : QChar('W'); }
	qreal loc() const;
	QPointF centerInScene() const;

	void setHighlighted(bool on);

	enum { Type = QGraphicsItem::UserType + 2 };
	int type() const override { return Type; }

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
	QString m_name;
	Direction m_dir;
	UnitOperationItem* m_unit;
	bool m_hovered = false;
};