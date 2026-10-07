#include "StreamItem.h"
#include "PortItem.h"
#include "UnitOperationItem.h"
#include "StreamBlockItem.h"
#include "FlowsheetScene.h"
#include "../simulationmanager.h"
#include "../dialogs/unitinfodialog.h"

#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <algorithm>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

static constexpr qreal DIST = 50.0;
static constexpr qreal MARGIN = 10.0;
static constexpr qreal SNAP_DIST = 20.0;

StreamItem::StreamItem(PortItem* from, PortItem* to, QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
	, m_source(from)
	, m_dest(to)
{
	setZValue(-1);
	setActive(false);
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	updatePath();
}

StreamItem::StreamItem(const QPointF& start, const QPointF& end, QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
	, m_freeStart(start)
	, m_freeEnd(end)
{
	setZValue(-1);
	setActive(false);
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	updatePath();
}

QPointF StreamItem::startPoint() const
{
	if (m_source) return m_source->centerInScene();
	return m_freeStart;
}

QPointF StreamItem::endPoint() const
{
	if (m_dest) return m_dest->centerInScene();
	return m_freeEnd;
}

void StreamItem::connectStart(PortItem* port)
{
	m_source = port;
	updatePath();
}

void StreamItem::connectEnd(PortItem* port)
{
	m_dest = port;
	updatePath();
}

void StreamItem::disconnectStart()
{
	if (m_source) {
		m_freeStart = m_source->centerInScene();
		m_source = nullptr;
	}
	updatePath();
}

void StreamItem::disconnectEnd()
{
	if (m_dest) {
		m_freeEnd = m_dest->centerInScene();
		m_dest = nullptr;
	}
	updatePath();
}

bool StreamItem::isConnected() const
{
	return m_source != nullptr && m_dest != nullptr;
}

void StreamItem::updatePath()
{
	QPointF sp = startPoint();
	QPointF ep = endPoint();

	if (isConnected()) {
		QRectF rectA = m_source->unitItem() ? m_source->unitItem()->sceneBoundingRect()
			: QRectF(sp, QSizeF(1, 1));
		QRectF rectB = m_dest->unitItem() ? m_dest->unitItem()->sceneBoundingRect()
			: QRectF(ep, QSizeF(1, 1));

		char sideA = m_source->side().toLatin1();
		char sideB = m_dest->side().toLatin1();
		qreal locA = m_source->loc();
		qreal locB = m_dest->loc();

		QList<QPointF> pts = routePath(rectA, rectB, sp, ep, sideA, sideB, locA, locB);
		buildPath(pts);
	} else {
		QList<QPointF> pts = { sp, ep };
		buildPath(pts);
	}

	updateLabel();
}

void StreamItem::buildPath(const QList<QPointF>& pts)
{
	if (pts.size() < 2) return;

	QPainterPath path;
	path.moveTo(pts[0]);
	for (int i = 1; i < pts.size(); ++i)
		path.lineTo(pts[i]);
	setPath(path);
}

QList<StreamSegment> StreamItem::segments() const
{
	QList<StreamSegment> segs;
	QPainterPath p = path();
	if (p.isEmpty()) return segs;

	QPolygonF poly = p.toFillPolygon();
	for (int i = 0; i < poly.size() - 1; ++i) {
		StreamSegment seg;
		seg.a = poly[i];
		seg.b = poly[i + 1];

		qreal dx = qAbs(seg.b.x() - seg.a.x());
		qreal dy = qAbs(seg.b.y() - seg.a.y());

		if (dx < 0.001 && dy < 0.001) {
			seg.valid = false;
		} else if (dy < 0.001) {
			seg.horizontal = true;
			seg.valid = true;
		} else if (dx < 0.001) {
			seg.horizontal = false;
			seg.valid = true;
		} else {
			seg.valid = false;
		}
		segs.append(seg);
	}
	return segs;
}

QList<StreamItem::Crossing> StreamItem::findCrossings() const
{
	QList<Crossing> result;
	if (!scene()) return result;

	QList<StreamSegment> mySegs = segments();
	if (mySegs.isEmpty()) return result;

	qreal hopR = 4.0 + pen().widthF() * 0.5;

	for (auto* item : scene()->items()) {
		auto* other = dynamic_cast<StreamItem*>(item);
		if (!other || other == this) continue;

		if (reinterpret_cast<uintptr_t>(this) < reinterpret_cast<uintptr_t>(other))
			continue;

		QList<StreamSegment> oSegs = other->segments();
		if (oSegs.isEmpty()) continue;

		for (const auto& ms : mySegs) {
			if (!ms.valid) continue;

			for (const auto& os : oSegs) {
				if (!os.valid) continue;
				if (ms.horizontal == os.horizontal) continue;

				if (ms.horizontal) {
					qreal y = ms.a.y();
					qreal minX = qMin(ms.a.x(), ms.b.x());
					qreal maxX = qMax(ms.a.x(), ms.b.x());
					qreal x = os.a.x();
					qreal minY = qMin(os.a.y(), os.b.y());
					qreal maxY = qMax(os.a.y(), os.b.y());

					if (x > minX + hopR - 0.5 && x < maxX - hopR + 0.5 &&
						y > minY + hopR - 0.5 && y < maxY - hopR + 0.5) {
						Crossing c;
						c.pt = QPointF(x, y);
						c.onHorizontal = true;
						result.append(c);
					}
				} else {
					qreal x = ms.a.x();
					qreal minY = qMin(ms.a.y(), ms.b.y());
					qreal maxY = qMax(ms.a.y(), ms.b.y());
					qreal y = os.a.y();
					qreal minX = qMin(os.a.x(), os.b.x());
					qreal maxX = qMax(os.a.x(), os.b.x());

					if (y > minY + hopR - 0.5 && y < maxY - hopR + 0.5 &&
						x > minX + hopR - 0.5 && x < maxX - hopR + 0.5) {
						Crossing c;
						c.pt = QPointF(x, y);
						c.onHorizontal = false;
						result.append(c);
					}
				}
			}
		}
	}
	return result;
}

QPainterPath StreamItem::shape() const
{
	QPainterPath p = path();
	if (p.isEmpty()) return p;

	QPainterPathStroker stroker;
	stroker.setWidth(HANDLE_RADIUS * 2);
	return stroker.createStroke(p);
}

void StreamItem::drawArrow(QPainter* painter, const QPolygonF& poly)
{
	if (poly.size() < 2) return;

	qreal totalLen = 0.0;
	for (int i = 0; i < poly.size() - 1; ++i) {
		QPointF d = poly[i + 1] - poly[i];
		totalLen += std::sqrt(d.x() * d.x() + d.y() * d.y());
	}
	if (totalLen < 1.0) return;

	qreal halfLen = totalLen * 0.5;
	qreal accum = 0.0;
	int segIdx = 0;
	for (int i = 0; i < poly.size() - 1; ++i) {
		QPointF d = poly[i + 1] - poly[i];
		qreal segLen = std::sqrt(d.x() * d.x() + d.y() * d.y());
		if (accum + segLen >= halfLen || i == poly.size() - 2) {
			segIdx = i;
			break;
		}
		accum += segLen;
	}

	QPointF d = poly[segIdx + 1] - poly[segIdx];
	qreal segLen = std::sqrt(d.x() * d.x() + d.y() * d.y());
	if (segLen < 1.0) return;
	QPointF segDir = d / segLen;
	qreal angle = std::atan2(segDir.y(), segDir.x());

	qreal t = (segLen > 0.001) ? (halfLen - accum) / segLen : 0.0;
	QPointF mid = poly[segIdx] + segDir * (t * segLen);

	qreal s = ARROW_SIZE * 3.0;

	QPointF pts[7] = {
		mid + QPointF(std::cos(angle) * (-s / 2) - std::sin(angle) * (-s / 12),
					  std::sin(angle) * (-s / 2) + std::cos(angle) * (-s / 12)),
		mid + QPointF(std::cos(angle) * (s / 9) - std::sin(angle) * (-s / 12),
					  std::sin(angle) * (s / 9) + std::cos(angle) * (-s / 12)),
		mid + QPointF(std::cos(angle) * (s / 9) - std::sin(angle) * (-7 * s / 36),
					  std::sin(angle) * (s / 9) + std::cos(angle) * (-7 * s / 36)),
		mid + QPointF(std::cos(angle) * (s / 2) - std::sin(angle) * 0,
					  std::sin(angle) * (s / 2) + std::cos(angle) * 0),
		mid + QPointF(std::cos(angle) * (s / 9) - std::sin(angle) * (7 * s / 36),
					  std::sin(angle) * (s / 9) + std::cos(angle) * (7 * s / 36)),
		mid + QPointF(std::cos(angle) * (s / 9) - std::sin(angle) * (s / 12),
					  std::sin(angle) * (s / 9) + std::cos(angle) * (s / 12)),
		mid + QPointF(std::cos(angle) * (-s / 2) - std::sin(angle) * (s / 12),
					  std::sin(angle) * (-s / 2) + std::cos(angle) * (s / 12)),
	};

	QPolygonF arrow;
	for (int i = 0; i < 7; ++i)
		arrow << pts[i];

	QColor fillColor = pen().color();
	fillColor.setAlpha(80);

	painter->setPen(QPen(pen().color(), 1.2));
	painter->setBrush(fillColor);
	painter->drawPolygon(arrow);
}

void StreamItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
	QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setRenderHint(QPainter::Antialiasing);

	QPen linePen = pen();
	qreal hopR = 4.0 + linePen.widthF() * 0.5;

	QList<StreamSegment> segs = segments();
	if (segs.isEmpty()) {
		painter->setPen(linePen);
		painter->drawPath(path());
		return;
	}

	QList<Crossing> crossings = findCrossings();

	painter->setPen(linePen);
	painter->setBrush(Qt::NoBrush);

	for (const auto& seg : segs) {
		if (!seg.valid) continue;

		QList<qreal> crossVals;
		for (const auto& c : crossings) {
			if (seg.horizontal && c.onHorizontal) {
				qreal minX = qMin(seg.a.x(), seg.b.x());
				qreal maxX = qMax(seg.a.x(), seg.b.x());
				if (qAbs(c.pt.y() - seg.a.y()) < 1.0 &&
					c.pt.x() > minX + hopR && c.pt.x() < maxX - hopR)
					crossVals.append(c.pt.x());
			} else if (!seg.horizontal && !c.onHorizontal) {
				qreal minY = qMin(seg.a.y(), seg.b.y());
				qreal maxY = qMax(seg.a.y(), seg.b.y());
				if (qAbs(c.pt.x() - seg.a.x()) < 1.0 &&
					c.pt.y() > minY + hopR && c.pt.y() < maxY - hopR)
					crossVals.append(c.pt.y());
			}
		}

		if (crossVals.isEmpty()) {
			painter->drawLine(seg.a, seg.b);
			continue;
		}

		if (seg.horizontal) {
			int dir = (seg.b.x() > seg.a.x()) ? 1 : -1;
			std::sort(crossVals.begin(), crossVals.end());
			if (dir < 0) std::reverse(crossVals.begin(), crossVals.end());

			qreal curX = seg.a.x();
			qreal curY = seg.a.y();
			for (qreal cx : crossVals) {
				painter->drawLine(QPointF(curX, curY),
					QPointF(cx - hopR * dir, curY));
				QRectF arcRect(cx - hopR, curY - hopR, hopR * 2, hopR * 2);
				painter->drawArc(arcRect, 0, 180 * 16);
				curX = cx + hopR * dir;
			}
			painter->drawLine(QPointF(curX, curY), seg.b);
		} else {
			int dir = (seg.b.y() > seg.a.y()) ? 1 : -1;
			std::sort(crossVals.begin(), crossVals.end());
			if (dir < 0) std::reverse(crossVals.begin(), crossVals.end());

			qreal curX = seg.a.x();
			qreal curY = seg.a.y();
			for (qreal cy : crossVals) {
				painter->drawLine(QPointF(curX, curY),
					QPointF(curX, cy - hopR * dir));
				QRectF arcRect(curX - hopR, cy - hopR, hopR * 2, hopR * 2);
				painter->drawArc(arcRect, 90 * 16, 180 * 16);
				curY = cy + hopR * dir;
			}
			painter->drawLine(QPointF(curX, curY), seg.b);
		}
	}
}

QList<QPointF> StreamItem::routePath(const QRectF& rectA, const QRectF& rectB,
	const QPointF& ptA, const QPointF& ptB,
	char sideA, char sideB, qreal locA, qreal locB) const
{
	if (sideA == 'E' && sideB == 'W')
		return linkEW(rectA, rectB, ptA, ptB, locA, locB);
	if (sideA == 'W' && sideB == 'E')
		return mirrorX(linkEW(
			QRectF(-rectA.right(), rectA.y(), rectA.width(), rectA.height()),
			QRectF(-rectB.right(), rectB.y(), rectB.width(), rectB.height()),
			QPointF(-ptA.x(), ptA.y()),
			QPointF(-ptB.x(), ptB.y()),
			locA, locB));
	if (sideA == 'E' && sideB == 'E')
		return linkEW(rectA, rectB, ptA, ptB, locA, 1.0 - locB);
	if (sideA == 'W' && sideB == 'W')
		return mirrorX(linkEW(
			QRectF(-rectA.right(), rectA.y(), rectA.width(), rectA.height()),
			QRectF(-rectB.right(), rectB.y(), rectB.width(), rectB.height()),
			QPointF(-ptA.x(), ptA.y()),
			QPointF(-ptB.x(), ptB.y()),
			locA, 1.0 - locB));

	QList<QPointF> pts = { ptA };
	qreal midX = (ptA.x() + ptB.x()) * 0.5;
	pts.append(QPointF(midX, ptA.y()));
	pts.append(QPointF(midX, ptB.y()));
	pts.append(ptB);
	return pts;
}

QList<QPointF> StreamItem::linkEW(const QRectF& ra, const QRectF& rb,
	const QPointF& pa, const QPointF& pb,
	qreal la, qreal lb) const
{
	QList<QPointF> pts;
	pts.append(pa);

	if (pb.x() > pa.x()) {
		pts.append(QPointF(pb.x() - DIST * lb, pa.y()));
		pts.append(QPointF(pb.x() - DIST * lb, pb.y()));
	} else {
		if (pb.y() < pa.y()) {
			pts.append(QPointF(pa.x() + DIST * la, pa.y()));
			pts.append(QPointF(pa.x() + DIST * la, ra.top() - DIST * la));
			pts.append(QPointF(pb.x() - DIST * lb, ra.top() - DIST * la));
			pts.append(QPointF(pb.x() - DIST * lb, pb.y()));
		} else {
			pts.append(QPointF(pa.x() + DIST * la, pa.y()));
			pts.append(QPointF(pa.x() + DIST * la, ra.bottom() + DIST * la));
			pts.append(QPointF(pb.x() - DIST * lb, ra.bottom() + DIST * la));
			pts.append(QPointF(pb.x() - DIST * lb, pb.y()));
		}
	}

	pts.append(pb);
	return pts;
}

QList<QPointF> StreamItem::mirrorX(const QList<QPointF>& pts)
{
	QList<QPointF> result;
	for (const auto& p : pts)
		result.append(QPointF(-p.x(), p.y()));
	return result;
}

QList<QPointF> StreamItem::mirrorY(const QList<QPointF>& pts)
{
	QList<QPointF> result;
	for (const auto& p : pts)
		result.append(QPointF(p.x(), -p.y()));
	return result;
}

StreamItem::DragHandle StreamItem::hitHandle(const QPointF& scenePos) const
{
	QPointF sp = startPoint();
	QPointF ep = endPoint();
	qreal dStart = QLineF(scenePos, sp).length();
	qreal dEnd = QLineF(scenePos, ep).length();

	if (dStart < HANDLE_RADIUS && dStart < dEnd)
		return DragHandle::Start;
	if (dEnd < HANDLE_RADIUS)
		return DragHandle::End;
	return DragHandle::None;
}

void StreamItem::setActive(bool active)
{
	m_active = active;
	QPen pen;
	if (active) {
		pen = QPen(QColor(0x00, 0xc8, 0x53), 3);
	} else {
		pen = QPen(QColor(0x89, 0xb4, 0xfa), 2);
		pen.setStyle(Qt::SolidLine);
	}
	setPen(pen);
}

void StreamItem::setMaterialStream(std::shared_ptr<ChemEngine::COBIA::CapeMaterialStream> stream)
{
	m_materialStream = stream;
	updateLabel();
}

QString StreamItem::streamName() const
{
	if (m_materialStream)
		return QString::fromStdWString(m_materialStream->getObjectName());
	return QString();
}

void StreamItem::updateLabel()
{
	if (m_label) {
		scene()->removeItem(m_label);
		delete m_label;
		m_label = nullptr;
	}

	QString labelText;
	if (m_materialStream)
		labelText = QString::fromStdWString(m_materialStream->getObjectName());
	else if (!m_associatedStreamName.empty())
		labelText = QString::fromStdWString(m_associatedStreamName);

	if (labelText.isEmpty()) return;

	QPointF sp = startPoint();
	QPointF ep = endPoint();
	QPointF mid = (sp + ep) * 0.5;
	mid += QPointF(0, -14);

	m_label = new QGraphicsTextItem(this);
	m_label->setPlainText(labelText);
	m_label->setDefaultTextColor(QColor(0x89, 0xb4, 0xfa));
	QFont font;
	font.setPointSize(7);
	m_label->setFont(font);
	m_label->setPos(mapFromScene(mid));
}

void StreamItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		DragHandle h = hitHandle(event->scenePos());
		if (h != DragHandle::None) {
			m_dragHandle = h;
			m_dragLastPos = event->scenePos();
			event->accept();
			return;
		}
	}
	QGraphicsPathItem::mousePressEvent(event);
}

void StreamItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_dragHandle != DragHandle::None) {
		m_dragLastPos = event->scenePos();

		if (m_dragHandle == DragHandle::Start) {
			m_freeStart = event->scenePos();
			m_source = nullptr;
		} else {
			m_freeEnd = event->scenePos();
			m_dest = nullptr;
		}
		updatePath();
		event->accept();
		return;
	}
	QGraphicsPathItem::mouseMoveEvent(event);
}

void StreamItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_dragHandle != DragHandle::None) {
		QPointF releasePos = event->scenePos();

		if (scene()) {
			auto itemsAtPos = scene()->items(releasePos, Qt::IntersectsItemShape, Qt::DescendingOrder);
			PortItem* snapped = nullptr;
			for (auto* item : itemsAtPos) {
				auto* port = dynamic_cast<PortItem*>(item);
				if (port) {
					QPointF portCenter = port->centerInScene();
					if (QLineF(releasePos, portCenter).length() < SNAP_DIST) {
						snapped = port;
						break;
					}
				}
			}

			if (snapped) {
				if (m_dragHandle == DragHandle::Start) {
					if (snapped->direction() == PortItem::Direction::Output) {
						m_source = snapped;
						m_freeStart = QPointF();
					}
				} else {
					if (snapped->direction() == PortItem::Direction::Input) {
						m_dest = snapped;
						m_freeEnd = QPointF();
					}
				}
			}
		}

		if (m_dragHandle == DragHandle::Start && !m_source) {
			m_freeStart = releasePos;
		}
		if (m_dragHandle == DragHandle::End && !m_dest) {
			m_freeEnd = releasePos;
		}

		m_dragHandle = DragHandle::None;
		updatePath();

		auto* fsScene = qobject_cast<FlowsheetScene*>(scene());
		if (fsScene) {
			emit fsScene->flowsheetChanged();
		}

		event->accept();
		return;
	}
	QGraphicsPathItem::mouseReleaseEvent(event);
}

void StreamItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	Q_UNUSED(event);

	auto* fsScene = qobject_cast<FlowsheetScene*>(scene());
	if (!fsScene) return;

	auto* simMgr = fsScene->simulationManager();
	if (!simMgr) return;

	auto* fs = simMgr->flowsheet();
	if (!fs) return;

	std::shared_ptr<ChemEngine::COBIA::CapeMaterialStream> matStream = m_materialStream;

	if (!matStream && !m_associatedStreamName.empty())
	{
		auto obj = fs->findObject(m_associatedStreamName);
		matStream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(obj);
		if (matStream)
			m_materialStream = matStream;
	}

	if (!matStream) return;

	bool isFeed = false;

	if (m_source && m_dest)
	{
		auto* srcParent = m_source->parentItem();
		auto* dstParent = m_dest->parentItem();

		bool srcIsBlock = srcParent && (dynamic_cast<StreamBlockItem*>(srcParent) != nullptr);
		bool dstIsUnitOp = dstParent && (dynamic_cast<UnitOperationItem*>(dstParent) != nullptr)
			&& (dynamic_cast<StreamBlockItem*>(dstParent) == nullptr);

		isFeed = srcIsBlock && dstIsUnitOp;
	}
	else if (!m_source && m_dest)
	{
		auto* dstParent = m_dest->parentItem();
		bool dstIsUnitOp = dstParent && (dynamic_cast<UnitOperationItem*>(dstParent) != nullptr)
			&& (dynamic_cast<StreamBlockItem*>(dstParent) == nullptr);
		isFeed = dstIsUnitOp;
	}

	auto* dlg = new UnitInfoDialog(simMgr);
	dlg->setAttribute(Qt::WA_DeleteOnClose);

	if (isFeed)
		dlg->loadFeedStreamEditor(*matStream);
	else
		dlg->loadStreamResults(*matStream);

	dlg->show();
}