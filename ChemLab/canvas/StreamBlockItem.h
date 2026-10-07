#pragma once

#include "UnitOperationItem.h"

class StreamBlockItem : public UnitOperationItem
{
public:
	explicit StreamBlockItem(const QString& name, FlowsheetScene* scene,
		QGraphicsItem* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	QPainterPath shape() const override;
	int type() const override { return Type; }

	enum { Type = QGraphicsItem::UserType + 4 };

	void setStreamName(const std::wstring& name) { m_streamName = name; }
	std::wstring streamName() const { return m_streamName; }

	static constexpr qreal ARROW_W = 42.0;
	static constexpr qreal ARROW_H = 16.33;
	static constexpr qreal PAD = 6.0;

protected:
	void setupAppearance() override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
	QPolygonF arrowPolygon() const;
	std::wstring m_streamName;
};