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

static constexpr qreal SNAP_DIST = 20.0;

StreamItem::StreamItem(PortItem* from, PortItem* to, QGraphicsItem* parent)
	: QGraphicsPathItem(parent)
	, m_source(from)
	, m_dest(to)
	, m_routingCtx(cr_routing_create("orthogonal"))
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
	, m_routingCtx(cr_routing_create("orthogonal"))
{
	setZValue(-1);
	setActive(false);
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	updatePath();
}

StreamItem::~StreamItem()
{
	cr_routing_destroy(m_routingCtx);
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

	if (isConnected() && m_routingCtx) {
		QRectF rectA = m_source->unitItem() ? m_source->unitItem()->sceneBoundingRect()
			: QRectF(sp, QSizeF(1, 1));
		QRectF rectB = m_dest->unitItem() ? m_dest->unitItem()->sceneBoundingRect()
			: QRectF(ep, QSizeF(1, 1));

		char sideA = m_source->side().toLatin1();
		char sideB = m_dest->side().toLatin1();

		auto toCrSide = [](char c) -> cr_side_t {
			switch (c) {
			case 'E': return CR_SIDE_E;
			case 'W': return CR_SIDE_W;
			case 'N': return CR_SIDE_N;
			case 'S': return CR_SIDE_S;
			default:  return CR_SIDE_E;
			}
		};

		cr_rect_t cra = { rectA.x(), rectA.y(), rectA.width(), rectA.height() };
		cr_rect_t crb = { rectB.x(), rectB.y(), rectB.width(), rectB.height() };
		cr_point_t cpa = { sp.x(), sp.y() };
		cr_point_t cpb = { ep.x(), ep.y() };

		cr_point_t out[32];
		int n = cr_routing_compute(m_routingCtx,
			&cra, &crb, &cpa, &cpb,
			toCrSide(sideA), toCrSide(sideB),
			m_source->loc(), m_dest->loc(),
			out, 32);

		m_cachedPts.clear();
		for (int i = 0; i < n && i < 32; i++)
			m_cachedPts.append(QPointF(out[i].x, out[i].y));
	} else {
		m_cachedPts = { sp, ep };
	}

	buildPath(m_cachedPts);
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
	int npts = m_cachedPts.size();
	if (npts < 2) return segs;

	cr_point_t pts[64];
	cr_segment_t csegs[64];
	int n = (npts < 64) ? npts : 64;
	for (int i = 0; i < n; i++) {
		pts[i].x = m_cachedPts[i].x();
		pts[i].y = m_cachedPts[i].y();
	}

	int count = cr_segments_from_polyline(pts, n, csegs, 64);
	for (int i = 0; i < count; i++) {
		StreamSegment seg;
		seg.a = QPointF(csegs[i].a.x, csegs[i].a.y);
		seg.b = QPointF(csegs[i].b.x, csegs[i].b.y);
		seg.horizontal = (csegs[i].horizontal == 1);
		seg.valid      = (csegs[i].valid == 1);
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

	cr_segment_t myCSegs[64];
	for (int i = 0; i < mySegs.size() && i < 64; i++) {
		myCSegs[i].a.x = mySegs[i].a.x();
		myCSegs[i].a.y = mySegs[i].a.y();
		myCSegs[i].b.x = mySegs[i].b.x();
		myCSegs[i].b.y = mySegs[i].b.y();
		myCSegs[i].horizontal = mySegs[i].horizontal ? 1 : 0;
		myCSegs[i].valid       = mySegs[i].valid ? 1 : 0;
	}

	for (auto* item : scene()->items()) {
		auto* other = dynamic_cast<StreamItem*>(item);
		if (!other || other == this) continue;

		if (reinterpret_cast<uintptr_t>(this) < reinterpret_cast<uintptr_t>(other))
			continue;

		QList<StreamSegment> oSegs = other->segments();
		if (oSegs.isEmpty()) continue;

		cr_segment_t oCSegs[64];
		for (int i = 0; i < oSegs.size() && i < 64; i++) {
			oCSegs[i].a.x = oSegs[i].a.x();
			oCSegs[i].a.y = oSegs[i].a.y();
			oCSegs[i].b.x = oSegs[i].b.x();
			oCSegs[i].b.y = oSegs[i].b.y();
			oCSegs[i].horizontal = oSegs[i].horizontal ? 1 : 0;
			oCSegs[i].valid       = oSegs[i].valid ? 1 : 0;
		}

		cr_crossing_t out[64];
		int n = cr_find_crossings(myCSegs, mySegs.size(),
			oCSegs, oSegs.size(), hopR, out, 64);

		for (int i = 0; i < n; i++) {
			Crossing c;
			c.pt = QPointF(out[i].pt.x, out[i].pt.y);
			c.onHorizontal = (out[i].on_horizontal == 1);
			result.append(c);
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