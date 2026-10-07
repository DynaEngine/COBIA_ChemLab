#include "FlowsheetScene.h"
#include "UnitOperationItem.h"
#include "PortItem.h"
#include "StreamItem.h"
#include "StreamBlockItem.h"
#include "../simulationmanager.h"

#include "ChemEngine/Flowsheet/Flowsheet.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsLineItem>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

FlowsheetScene::FlowsheetScene(SimulationManager* simMgr, QObject* parent)
	: QGraphicsScene(parent)
	, m_simMgr(simMgr)
{
	setSceneRect(-2000, -2000, 4000, 4000);
	setBackgroundBrush(QColor(0x1e, 0x1e, 0x2e));
}

void FlowsheetScene::setMode(Mode mode)
{
	m_mode = mode;
	if (m_mode == Mode::Connect)
	{
		for (auto* item : m_unitItems)
		{
			item->setFlag(QGraphicsItem::ItemIsMovable, false);
			item->setFlag(QGraphicsItem::ItemIsSelectable, false);
		}
	}
	else
	{
		for (auto* item : m_unitItems)
		{
			item->setFlag(QGraphicsItem::ItemIsMovable, true);
			item->setFlag(QGraphicsItem::ItemIsSelectable, true);
		}
	}

	if (m_mode != Mode::Connect)
	{
		cancelConnection();
	}
}

UnitOperationItem* FlowsheetScene::addUnitOperation(const QString& name, const QString& typeName,
	const QStringList& inPorts, const QStringList& outPorts)
{
	auto* item = new UnitOperationItem(name, typeName, inPorts, outPorts, this);
	addItem(item);

	QPointF pos(0, 0);
	int count = m_unitItems.size();
	pos.setX(50.0 + (count % 3) * 200.0);
	pos.setY(50.0 + (count / 3) * 150.0);
	item->setPos(pos);

	m_unitItems[name.toLower()] = item;

	if (m_mode == Mode::Connect)
	{
		item->setFlag(QGraphicsItem::ItemIsMovable, false);
		item->setFlag(QGraphicsItem::ItemIsSelectable, false);
	}

	emit flowsheetChanged();
	return item;
}

void FlowsheetScene::removeUnitOperation(UnitOperationItem* item)
{
	if (!item) return;

	QList<StreamItem*> toRemove;
	QSet<StreamBlockItem*> blocksToRemove;

	for (auto* stream : m_streamItems)
	{
		if (stream->sourcePort() && stream->sourcePort()->unitItem() == item)
		{
			toRemove.append(stream);
			auto* block = dynamic_cast<StreamBlockItem*>(
				stream->destPort() ? stream->destPort()->unitItem() : nullptr);
			if (block) blocksToRemove.insert(block);
		}
		if (stream->destPort() && stream->destPort()->unitItem() == item)
		{
			toRemove.append(stream);
			auto* block = dynamic_cast<StreamBlockItem*>(
				stream->sourcePort() ? stream->sourcePort()->unitItem() : nullptr);
			if (block) blocksToRemove.insert(block);
		}
	}

	for (auto* block : blocksToRemove)
	{
		for (auto* s : m_streamItems)
		{
			if (s->sourcePort() && s->sourcePort()->unitItem() == block)
				toRemove.append(s);
			else if (s->destPort() && s->destPort()->unitItem() == block)
				toRemove.append(s);
		}
		m_streamBlocks.removeOne(block);
		removeItem(block);
		delete block;
	}

	for (auto* stream : toRemove)
	{
		m_streamItems.removeOne(stream);
		removeItem(stream);
		delete stream;
	}

	m_unitItems.remove(item->name().toLower());
	removeItem(item);
	delete item;
	emit flowsheetChanged();
}

void FlowsheetScene::connectPorts(PortItem* from, PortItem* to)
{
	if (!from || !to) return;
	if (from->direction() != PortItem::Direction::Output) return;
	if (to->direction() != PortItem::Direction::Input) return;

	bool fromIsBlock = dynamic_cast<StreamBlockItem*>(from->unitItem()) != nullptr;
	bool toIsBlock = dynamic_cast<StreamBlockItem*>(to->unitItem()) != nullptr;

	if (fromIsBlock && toIsBlock) return;

	std::wstring fromObj = from->unitItem()->name().toStdWString();
	std::wstring fromPortName = from->portName().toStdWString();
	std::wstring toObj = to->unitItem()->name().toStdWString();
	std::wstring toPortName = to->portName().toStdWString();

	for (auto* s : m_streamItems)
	{
		if (s->destPort() == to)
		{
			StreamBlockItem* removedBlock = nullptr;
			if (s->sourcePort())
				removedBlock = dynamic_cast<StreamBlockItem*>(s->sourcePort()->unitItem());

			if (removedBlock)
			{
				QList<StreamItem*> blockStreams;
				for (auto* bs : m_streamItems)
				{
					if (bs->sourcePort() && bs->sourcePort()->unitItem() == removedBlock)
						blockStreams.append(bs);
					else if (bs->destPort() && bs->destPort()->unitItem() == removedBlock)
						blockStreams.append(bs);
				}
				for (auto* bs : blockStreams)
				{
					m_streamItems.removeOne(bs);
					removeItem(bs);
					delete bs;
				}
				m_streamBlocks.removeOne(removedBlock);
				removeItem(removedBlock);
				delete removedBlock;
			}
			m_streamItems.removeOne(s);
			removeItem(s);
			delete s;
			break;
		}
	}

	if (fromIsBlock || toIsBlock)
	{
		auto* stream = new StreamItem(from, to);
		addItem(stream);
		m_streamItems.append(stream);

		if (fromIsBlock && !toIsBlock)
		{
			auto* srcBlock = dynamic_cast<StreamBlockItem*>(from->unitItem());
			std::wstring msName = srcBlock ? srcBlock->streamName() : L"";
			if (!msName.empty() && m_simMgr && m_simMgr->flowsheet())
			{
				auto obj = m_simMgr->flowsheet()->findObject(msName);
				auto matStream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(obj);
				if (matStream)
				{
					stream->setMaterialStream(matStream);
					stream->setAssociatedStreamName(msName);
				}
				m_simMgr->connectFlowsheetObjects(msName, L"", toObj, toPortName);
			}
		}
		else if (!fromIsBlock && toIsBlock)
		{
			auto* dstBlock = dynamic_cast<StreamBlockItem*>(to->unitItem());
			std::wstring msName = dstBlock ? dstBlock->streamName() : L"";
			if (!msName.empty() && m_simMgr && m_simMgr->flowsheet())
			{
				auto obj = m_simMgr->flowsheet()->findObject(msName);
				auto matStream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(obj);
				if (matStream)
				{
					stream->setMaterialStream(matStream);
					stream->setAssociatedStreamName(msName);
				}
				m_simMgr->connectFlowsheetObjects(fromObj, fromPortName, msName, L"");
			}
		}

		emit flowsheetChanged();
		return;
	}

	{
		QPointF fromPt = from->centerInScene();
		QPointF toPt = to->centerInScene();
		QPointF blockCenter = (fromPt + toPt) / 2.0;

		++m_streamCounter;
		QString blockName = QStringLiteral("S") + QString::number(m_streamCounter);
		std::wstring streamName = L"Stream" + std::to_wstring(m_streamCounter);

		auto* block = new StreamBlockItem(blockName, this);
		block->setStreamName(streamName);
		addItem(block);
		block->setPos(blockCenter.x() - block->rect().width() * 0.5,
					  blockCenter.y() - block->rect().height() * 0.5);
		m_streamBlocks.append(block);

		auto* s1 = new StreamItem(from, block->inputPorts().first());
		addItem(s1);
		m_streamItems.append(s1);

		auto* s2 = new StreamItem(block->outputPorts().first(), to);
		addItem(s2);
		m_streamItems.append(s2);

		if (m_simMgr)
		{
			m_simMgr->addMaterialStream(streamName);
			m_simMgr->connectFlowsheetObjects(fromObj, fromPortName, streamName, L"");
			m_simMgr->connectFlowsheetObjects(streamName, L"", toObj, toPortName);

			auto* fs = m_simMgr->flowsheet();
			if (fs)
			{
				auto obj = fs->findObject(streamName);
				auto matStream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(obj);
				if (matStream)
				{
					s1->setMaterialStream(matStream);
					s1->setAssociatedStreamName(streamName);
					s2->setMaterialStream(matStream);
					s2->setAssociatedStreamName(streamName);
				}
			}
		}
	}

	emit flowsheetChanged();
}

void FlowsheetScene::clearFlowsheet()
{
	cancelConnection();

	for (auto* stream : m_streamItems)
	{
		removeItem(stream);
		delete stream;
	}
	m_streamItems.clear();

	for (auto* block : m_streamBlocks)
	{
		removeItem(block);
		delete block;
	}
	m_streamBlocks.clear();

	for (auto it = m_unitItems.begin(); it != m_unitItems.end(); ++it)
	{
		removeItem(it.value());
		delete it.value();
	}
	m_unitItems.clear();
	m_streamCounter = 0;
	emit flowsheetChanged();
}

QList<UnitOperationItem*> FlowsheetScene::unitItems() const
{
	return m_unitItems.values();
}

void FlowsheetScene::startConnection(PortItem* outputPort)
{
	if (!outputPort) return;

	cancelConnection();

	m_pendingPort = outputPort;
	m_dragLine = new QGraphicsLineItem();
	QPen pen(QColor(0x4a, 0x9e, 0xff), 2, Qt::DashLine);
	m_dragLine->setPen(pen);
	m_dragLine->setZValue(100);
	QPointF from = outputPort->centerInScene();
	m_dragLine->setLine(QLineF(from, from));
	addItem(m_dragLine);

	emit statusMessage(QStringLiteral("Click an input port to connect (from %1)")
		.arg(outputPort->unitItem()->name()));
}

void FlowsheetScene::startTemporaryConnection(PortItem* outputPort)
{
	if (!outputPort) return;
	if (outputPort->direction() != PortItem::Direction::Output) return;

	m_temporaryConnect = true;
	setMode(Mode::Connect);
	startConnection(outputPort);
}

void FlowsheetScene::cancelConnection()
{
	m_pendingPort = nullptr;
	if (m_dragLine)
	{
		removeItem(m_dragLine);
		delete m_dragLine;
		m_dragLine = nullptr;
	}
}

StreamItem* FlowsheetScene::addStream(const QPointF& pos)
{
	QPointF start = pos + QPointF(-60, 0);
	QPointF end = pos + QPointF(60, 0);
	auto* stream = new StreamItem(start, end);
	addItem(stream);
	m_streamItems.append(stream);
	emit flowsheetChanged();
	return stream;
}

StreamBlockItem* FlowsheetScene::addStreamBlock(const QPointF& pos, const std::wstring& streamName)
{
	++m_streamCounter;
	QString name = QStringLiteral("S") + QString::number(m_streamCounter);
	auto* block = new StreamBlockItem(name, this);
	block->setPos(pos.x() - block->rect().width() * 0.5,
				  pos.y() - block->rect().height() * 0.5);
	if (!streamName.empty())
		block->setStreamName(streamName);
	addItem(block);
	m_streamBlocks.append(block);
	emit flowsheetChanged();
	return block;
}

void FlowsheetScene::removeStreamBlock(StreamBlockItem* block)
{
	if (!block) return;

	QList<StreamItem*> attached;
	for (auto* s : m_streamItems)
	{
		if (s->sourcePort() && s->sourcePort()->unitItem() == block)
			attached.append(s);
		else if (s->destPort() && s->destPort()->unitItem() == block)
			attached.append(s);
	}
	for (auto* s : attached)
	{
		m_streamItems.removeOne(s);
		removeItem(s);
		delete s;
	}

	m_streamBlocks.removeOne(block);
	removeItem(block);
	delete block;
	emit flowsheetChanged();
}

void FlowsheetScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_mode == Mode::Connect && event->button() == Qt::LeftButton)
	{
		if (m_pendingPort)
		{
			auto itemsAtPos = items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);

			PortItem* targetPort = nullptr;
			for (auto* item : itemsAtPos)
			{
				if (item == m_dragLine)
					continue;
				targetPort = dynamic_cast<PortItem*>(item);
				if (targetPort)
					break;
			}

			if (targetPort && targetPort->direction() == PortItem::Direction::Input
				&& targetPort->unitItem() != m_pendingPort->unitItem())
			{
				emit portConnectionRequested(m_pendingPort, targetPort);
				connectPorts(m_pendingPort, targetPort);
				emit statusMessage(QStringLiteral("Connected %1:%2 -> %3:%4")
					.arg(m_pendingPort->unitItem()->name())
					.arg(m_pendingPort->portName())
					.arg(targetPort->unitItem()->name())
					.arg(targetPort->portName()));
			}
			else
			{
				emit statusMessage(QStringLiteral("Connection cancelled"));
			}
			cancelConnection();

			if (m_temporaryConnect)
			{
				m_temporaryConnect = false;
				setMode(Mode::Select);
			}
			return;
		}
	}

	if (m_mode == Mode::Select && event->button() == Qt::LeftButton
		&& !(event->modifiers() & Qt::ControlModifier))
	{
		auto* item = itemAt(event->scenePos(), QTransform());
		auto* unitItem = dynamic_cast<UnitOperationItem*>(item);
		if (unitItem && !unitItem->isSelected())
		{
			clearSelection();
			unitItem->setSelected(true);
		}
	}

	QGraphicsScene::mousePressEvent(event);
}

void FlowsheetScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_mode == Mode::Connect && m_dragLine && m_pendingPort)
	{
		QPointF from = m_pendingPort->centerInScene();
		m_dragLine->setLine(QLineF(from, event->scenePos()));
		return;
	}

	QGraphicsScene::mouseMoveEvent(event);
}

void FlowsheetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_mode == Mode::Connect && m_dragLine && m_pendingPort)
		return;

	QGraphicsScene::mouseReleaseEvent(event);
}

void FlowsheetScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
	if (event->mimeData()->hasFormat(QStringLiteral("application/x-chemlab-unit")))
		event->acceptProposedAction();
	else
		QGraphicsScene::dragEnterEvent(event);
}

void FlowsheetScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
	if (event->mimeData()->hasFormat(QStringLiteral("application/x-chemlab-unit")))
		event->acceptProposedAction();
	else
		QGraphicsScene::dragMoveEvent(event);
}

void FlowsheetScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
	event->ignore();
}