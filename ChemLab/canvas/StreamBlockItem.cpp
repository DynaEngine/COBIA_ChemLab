#include "StreamBlockItem.h"
#include "PortItem.h"
#include "FlowsheetScene.h"
#include "../simulationmanager.h"
#include "../dialogs/unitinfodialog.h"

#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

StreamBlockItem::StreamBlockItem(const QString& name, FlowsheetScene* scene,
	QGraphicsItem* parent)
	: UnitOperationItem(name, QStringLiteral("Stream"), scene, parent)
{
	setRect(0, 0, ARROW_W + PAD * 2, ARROW_H + PAD * 2);

	QStringList inPorts, outPorts;
	inPorts.append(QStringLiteral("inlet"));
	outPorts.append(QStringLiteral("outlet"));
	createPorts(inPorts, outPorts);

	qreal cy = rect().height() * 0.5;
	for (auto* p : m_inputPorts)
		p->setPos(0, cy);
	for (auto* p : m_outputPorts)
		p->setPos(rect().width(), cy);
}

void StreamBlockItem::setupAppearance()
{
	m_label = nullptr;
}

void StreamBlockItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
	QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setRenderHint(QPainter::Antialiasing);

	QColor baseColor = isSelected() ? QColor(0xff, 0xd7, 0x00) : QColor(0x4a, 0x9e, 0xff);
	QColor fillColor(baseColor);
	fillColor.setAlpha(80);

	QPolygonF poly = arrowPolygon();

	painter->setPen(QPen(baseColor, isSelected() ? 2.5 : 1.8));
	painter->setBrush(fillColor);
	painter->drawPolygon(poly);
}

QPainterPath StreamBlockItem::shape() const
{
	QPainterPath path;
	path.addPolygon(arrowPolygon());
	return path;
}

QPolygonF StreamBlockItem::arrowPolygon() const
{
	qreal cx = rect().width() * 0.5;
	qreal cy = rect().height() * 0.5;
	qreal hw = ARROW_W * 0.5;
	qreal hh = ARROW_H * 0.5;
	qreal stepW = hw * 20.0 / 90.0;
	qreal stepH = hh * 15.0 / 35.0;

	QPolygonF poly;
	poly << QPointF(cx - hw,             cy - stepH);
	poly << QPointF(cx - hw + stepW * 5.5, cy - stepH);
	poly << QPointF(cx - hw + stepW * 5.5, cy - hh);
	poly << QPointF(cx + hw,             cy);
	poly << QPointF(cx - hw + stepW * 5.5, cy + hh);
	poly << QPointF(cx - hw + stepW * 5.5, cy + stepH);
	poly << QPointF(cx - hw,             cy + stepH);
	return poly;
}

void StreamBlockItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	Q_UNUSED(event);

	auto* fsScene = qobject_cast<FlowsheetScene*>(scene());
	if (!fsScene) return;

	auto* simMgr = fsScene->simulationManager();
	if (!simMgr) return;

	auto* fs = simMgr->flowsheet();
	if (!fs) return;

	ChemEngine::COBIA::CapeMaterialStream* matStream = nullptr;
	std::shared_ptr<ChemEngine::ISimulationObject> obj;

	if (!m_streamName.empty())
		obj = fs->findObject(m_streamName);
	else
		obj = fs->findObject(m_name.toStdWString());

	matStream = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(obj.get());

	if (!matStream) return;

	auto* dlg = new UnitInfoDialog(simMgr);
	dlg->setAttribute(Qt::WA_DeleteOnClose);

	dlg->loadFeedStreamEditor(*matStream);
	dlg->show();
}