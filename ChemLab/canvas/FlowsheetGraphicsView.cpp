#include "FlowsheetGraphicsView.h"
#include "FlowsheetScene.h"
#include "StreamItem.h"
#include "UnitOperationItem.h"
#include "../simulationmanager.h"

#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QtMath>

FlowsheetGraphicsView::FlowsheetGraphicsView(FlowsheetScene* scene, SimulationManager* simMgr, QWidget* parent)
	: QGraphicsView(scene, parent)
	, m_simMgr(simMgr)
{
	setRenderHint(QPainter::Antialiasing);
	setRenderHint(QPainter::SmoothPixmapTransform);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	QGraphicsView::setDragMode(QGraphicsView::RubberBandDrag);
	viewport()->setCursor(Qt::ArrowCursor);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);
	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setMinimumSize(400, 300);
	setFrameStyle(QFrame::NoFrame);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void FlowsheetGraphicsView::setDragMode(bool enabled)
{
	if (enabled)
		QGraphicsView::setDragMode(QGraphicsView::ScrollHandDrag);
	else
		QGraphicsView::setDragMode(QGraphicsView::RubberBandDrag);
}

void FlowsheetGraphicsView::wheelEvent(QWheelEvent* event)
{
	const qreal factor = 1.15;
	if (event->angleDelta().y() > 0)
	{
		if (transform().m11() < 5.0)
			scale(factor, factor);
	}
	else
	{
		if (transform().m11() > 0.2)
			scale(1.0 / factor, 1.0 / factor);
	}
}

void FlowsheetGraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		m_panning = true;
		m_lastPanPos = event->pos();
		setCursor(Qt::ClosedHandCursor);
		event->accept();
		return;
	}
	QGraphicsView::mousePressEvent(event);
}

void FlowsheetGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_panning)
	{
		QPoint delta = event->pos() - m_lastPanPos;
		m_lastPanPos = event->pos();
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
		event->accept();
		return;
	}
	QGraphicsView::mouseMoveEvent(event);
}

void FlowsheetGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton && m_panning)
	{
		m_panning = false;
		setCursor(Qt::ArrowCursor);
		event->accept();
		return;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void FlowsheetGraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("application/x-chemlab-unit"))
		event->acceptProposedAction();
	else
		QGraphicsView::dragEnterEvent(event);
}

void FlowsheetGraphicsView::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat("application/x-chemlab-unit"))
		event->acceptProposedAction();
	else
		QGraphicsView::dragMoveEvent(event);
}

void FlowsheetGraphicsView::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasFormat("application/x-chemlab-unit"))
	{
		QByteArray data = event->mimeData()->data("application/x-chemlab-unit");
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonObject obj = doc.object();

		QString displayName = obj["name"].toString();
		QString typeName = obj["type"].toString();
		QStringList inPorts, outPorts;
		for (const auto& v : obj["inPorts"].toArray())
			inPorts.append(v.toString());
		for (const auto& v : obj["outPorts"].toArray())
			outPorts.append(v.toString());

		QString category = obj["category"].toString();

		auto* scene = qobject_cast<FlowsheetScene*>(this->scene());

		if (typeName == QStringLiteral("materialstream"))
		{
			if (scene)
			{
				static int streamCounter = 1;
				std::wstring streamName = L"Stream" + std::to_wstring(streamCounter++);
				if (m_simMgr)
					m_simMgr->addMaterialStream(streamName);

				QPointF dropPos = mapToScene(event->position().toPoint());
				scene->addStreamBlock(dropPos, streamName);
			}
			event->acceptProposedAction();
			return;
		}

		static int unitCounter = 1;
		std::wstring uniqueName = (displayName + QString::number(unitCounter++)).toStdWString();

		if (category == QStringLiteral("builtin") || category == QStringLiteral("stream"))
		{
			if (m_simMgr)
				m_simMgr->loadBuiltInUnitOperation(typeName, uniqueName);
		}
		else
		{
			QString progId = obj["progId"].toString();
			if (m_simMgr)
			{
				m_simMgr->loadUnitOperation(progId.toStdWString(), uniqueName);

				auto* fs = m_simMgr->flowsheet();
				if (fs)
				{
					auto objPtr = fs->findObject(uniqueName);
					auto capeUnit = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeUnitWrapper>(objPtr);
					if (capeUnit)
					{
						inPorts.clear();
						outPorts.clear();
						for (const auto& p : capeUnit->getPorts())
						{
							QString pn = QString::fromStdWString(p.name);
							if (p.direction == CAPEOPEN_1_2::CAPE_INLET)
								inPorts.append(pn);
							else
								outPorts.append(pn);
						}
					}
				}
			}
		}

		if (scene)
		{
			QString sceneName = QString::fromStdWString(uniqueName);
			auto* item = scene->addUnitOperation(sceneName, displayName, inPorts, outPorts);
			if (item)
			{
				QPointF dropPos = mapToScene(event->position().toPoint());
				item->setPos(dropPos - QPointF(60, 35));
			}
		}
		event->acceptProposedAction();
		return;
	}
	QGraphicsView::dropEvent(event);
}

void FlowsheetGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
	QGraphicsView::drawBackground(painter, rect);

	painter->save();
	painter->setPen(QPen(QColor(0x31, 0x31, 0x44), 1));

	qreal left = int(rect.left()) - (int(rect.left()) % 40);
	qreal top = int(rect.top()) - (int(rect.top()) % 40);

	for (qreal x = left; x < rect.right(); x += 40)
		painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
	for (qreal y = top; y < rect.bottom(); y += 40)
		painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));

	painter->restore();
}