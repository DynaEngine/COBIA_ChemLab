#pragma once

#include <QGraphicsPathItem>
#include <QPen>
#include <QList>
#include <QPointF>
#include <QMap>
#include <memory>

class PortItem;
class UnitOperationItem;

namespace ChemEngine {
namespace COBIA {
	class CapeMaterialStream;
}
}

struct StreamSegment {
	QPointF a;
	QPointF b;
	bool horizontal;        // true=H, false=V, invalid for diagonal
	bool valid = true;      // false if degenerate (zero-length or diagonal)
};

class StreamItem : public QGraphicsPathItem
{
public:
	StreamItem(PortItem* from, PortItem* to, QGraphicsItem* parent = nullptr);
	StreamItem(const QPointF& start, const QPointF& end, QGraphicsItem* parent = nullptr);
	~StreamItem() override = default;

	PortItem* sourcePort() const { return m_source; }
	PortItem* destPort() const { return m_dest; }

	void connectStart(PortItem* port);
	void connectEnd(PortItem* port);
	void disconnectStart();
	void disconnectEnd();

	QPointF startPoint() const;
	QPointF endPoint() const;

	void updatePath();
	void setActive(bool active);
	bool isActive() const { return m_active; }

	void setMaterialStream(std::shared_ptr<ChemEngine::COBIA::CapeMaterialStream> stream);
	std::shared_ptr<ChemEngine::COBIA::CapeMaterialStream> materialStream() const { return m_materialStream; }
	QString streamName() const;

	void setAssociatedStreamName(const std::wstring& name) { m_associatedStreamName = name; }
	std::wstring associatedStreamName() const { return m_associatedStreamName; }

	QList<StreamSegment> segments() const;

	enum { Type = QGraphicsItem::UserType + 3 };
	int type() const override { return Type; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	QPainterPath shape() const override;

private:
	QList<QPointF> routePath(const QRectF& rectA, const QRectF& rectB,
		const QPointF& ptA, const QPointF& ptB,
		char sideA, char sideB, qreal locA, qreal locB) const;
	QList<QPointF> linkEW(const QRectF& ra, const QRectF& rb,
		const QPointF& pa, const QPointF& pb,
		qreal la, qreal lb) const;

	static QList<QPointF> mirrorX(const QList<QPointF>& pts);
	static QList<QPointF> mirrorY(const QList<QPointF>& pts);

	struct Crossing {
		QPointF pt;
		bool onHorizontal;
	};
	QList<Crossing> findCrossings() const;
	void updateLabel();
	void buildPath(const QList<QPointF>& pts);

	enum class DragHandle { None, Start, End };
	DragHandle hitHandle(const QPointF& scenePos) const;
	void drawArrow(QPainter* painter, const QPolygonF& poly);
	bool isConnected() const;

	PortItem* m_source = nullptr;
	PortItem* m_dest = nullptr;
	QPointF m_freeStart;
	QPointF m_freeEnd;
	bool m_active = false;
	DragHandle m_dragHandle = DragHandle::None;
	QPointF m_dragLastPos;
	std::shared_ptr<ChemEngine::COBIA::CapeMaterialStream> m_materialStream;
	std::wstring m_associatedStreamName;
	QGraphicsTextItem* m_label = nullptr;

	static constexpr qreal ARROW_SIZE = 14.0;
	static constexpr qreal HANDLE_RADIUS = 12.0;
};