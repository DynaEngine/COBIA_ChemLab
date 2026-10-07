#include "UnitOperationItem.h"
#include "PortItem.h"
#include "StreamItem.h"
#include "FlowsheetScene.h"
#include "../simulationmanager.h"
#include "../dialogs/unitinfodialog.h"

#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/Units/UnitOperationBase.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QTableWidget>

UnitOperationItem::UnitOperationItem(const QString& name, const QString& typeName,
	const QStringList& inPorts, const QStringList& outPorts,
	FlowsheetScene* scene, QGraphicsItem* parent)
	: QGraphicsRectItem(0, 0, WIDTH, HEIGHT, parent)
	, m_name(name)
	, m_typeName(typeName)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setCursor(Qt::ArrowCursor);

	setupAppearance();
	createPorts(inPorts, outPorts);
}

UnitOperationItem::UnitOperationItem(const QString& name, const QString& typeName,
	FlowsheetScene* scene, QGraphicsItem* parent)
	: QGraphicsRectItem(0, 0, 50, 24, parent)
	, m_name(name)
	, m_typeName(typeName)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setCursor(Qt::ArrowCursor);
}

void UnitOperationItem::setupAppearance()
{
	QPen pen(QColor(0x4a, 0x9e, 0xff), 2);
	setPen(pen);
	setBrush(QColor(0x31, 0x31, 0x44));
	setRect(0, 0, WIDTH, HEIGHT);

	m_label = new QGraphicsTextItem(this);
	m_label->setPlainText(m_name);
	m_label->setDefaultTextColor(QColor(0xcd, 0xd6, 0xf4));
	QFont font;
	font.setPointSize(8);
	font.setBold(true);
	m_label->setFont(font);

	QRectF textRect = m_label->boundingRect();
	m_label->setPos((WIDTH - textRect.width()) / 2.0, 6);

	auto* typeLabel = new QGraphicsTextItem(this);
	typeLabel->setPlainText(m_typeName);
	typeLabel->setDefaultTextColor(QColor(0xa6, 0xad, 0xc8));
	QFont typeFont;
	typeFont.setPointSize(7);
	typeLabel->setFont(typeFont);
	QRectF tRect = typeLabel->boundingRect();
	typeLabel->setPos((WIDTH - tRect.width()) / 2.0, 32);

	auto* inLabel = new QGraphicsTextItem(this);
	inLabel->setPlainText(QStringLiteral("IN"));
	inLabel->setDefaultTextColor(QColor(0x4a, 0x9e, 0xff));
	QFont smallFont;
	smallFont.setPointSize(6);
	inLabel->setFont(smallFont);
	inLabel->setPos(6, HEIGHT - 14);

	auto* outLabel = new QGraphicsTextItem(this);
	outLabel->setPlainText(QStringLiteral("OUT"));
	outLabel->setDefaultTextColor(QColor(0x00, 0xc8, 0x53));
	outLabel->setFont(smallFont);
	outLabel->setPos(WIDTH - 22, HEIGHT - 14);
}

void UnitOperationItem::createPorts(const QStringList& inPorts, const QStringList& outPorts)
{
	qreal yStep = HEIGHT / (inPorts.size() + 1);
	for (int i = 0; i < inPorts.size(); ++i)
	{
		auto* port = new PortItem(inPorts[i], PortItem::Direction::Input, this);
		port->setPos(0, yStep * (i + 1));
		m_inputPorts.append(port);
	}

	yStep = HEIGHT / (outPorts.size() + 1);
	for (int i = 0; i < outPorts.size(); ++i)
	{
		auto* port = new PortItem(outPorts[i], PortItem::Direction::Output, this);
		port->setPos(WIDTH, yStep * (i + 1));
		m_outputPorts.append(port);
	}
}

PortItem* UnitOperationItem::findPort(const QString& portName) const
{
	for (auto* p : m_inputPorts)
		if (p->portName().compare(portName, Qt::CaseInsensitive) == 0)
			return p;
	for (auto* p : m_outputPorts)
		if (p->portName().compare(portName, Qt::CaseInsensitive) == 0)
			return p;
	return nullptr;
}

void UnitOperationItem::setHighlighted(bool on)
{
	if (on)
	{
		setPen(QPen(QColor(0xff, 0xd7, 0x00), 3));
		setBrush(QColor(0x45, 0x45, 0x5e));
	}
	else
	{
		setPen(QPen(QColor(0x4a, 0x9e, 0xff), 2));
		setBrush(QColor(0x31, 0x31, 0x44));
	}
}

QVariant UnitOperationItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionHasChanged)
	{
		auto* scene = qobject_cast<FlowsheetScene*>(this->scene());
		if (scene)
		{
			for (auto* item : scene->items())
			{
				auto* stream = dynamic_cast<StreamItem*>(item);
				if (stream)
					stream->updatePath();
			}
		}
	}
	return QGraphicsRectItem::itemChange(change, value);
}

void UnitOperationItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsRectItem::mousePressEvent(event);
}

void UnitOperationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	auto* fsScene = qobject_cast<FlowsheetScene*>(scene());
	if (!fsScene) { QGraphicsRectItem::mouseDoubleClickEvent(event); return; }

	auto* simMgr = fsScene->simulationManager();
	if (!simMgr) { QGraphicsRectItem::mouseDoubleClickEvent(event); return; }

	auto* fs = simMgr->flowsheet();
	if (!fs) { QGraphicsRectItem::mouseDoubleClickEvent(event); return; }

	std::wstring objName = m_name.toStdWString();
	auto objPtr = fs->findObject(objName);

	if (objPtr)
	{
		auto* dlg = new UnitInfoDialog(simMgr);
		dlg->setAttribute(Qt::WA_DeleteOnClose);

		auto capeUnit = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeUnitWrapper>(objPtr);
		if (capeUnit)
		{
			ChemEngine::COBIA::UnitOperationInfo info;
			info.name = capeUnit->getObjectName();
			info.type = ChemEngine::COBIA::UnitOperationType::COBIA;
			dlg->loadUnitInfo(info);
			dlg->setWindowTitle(QStringLiteral("Unit Info - %1").arg(m_name));
			dlg->show();
			return;
		}

		auto builtIn = std::dynamic_pointer_cast<ChemEngine::UnitOperationBase>(objPtr);
		if (builtIn)
		{
			dlg->setWindowTitle(QStringLiteral("Unit Info - %1").arg(m_name));
			dlg->loadBuiltInUnitInfo(objName);
			dlg->show();
			return;
		}

		auto matStream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(objPtr);
		if (matStream)
		{
			dlg->setWindowTitle(QStringLiteral("Material Stream - %1").arg(m_name));
			dlg->loadStreamResults(*matStream);
			dlg->show();
			return;
		}
	}

	QGraphicsRectItem::mouseDoubleClickEvent(event);
}