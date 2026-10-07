#pragma once

#include <QGraphicsScene>
#include <QMap>
#include <QString>

class SimulationManager;
class UnitOperationItem;
class StreamItem;
class StreamBlockItem;
class PortItem;

class FlowsheetScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum class Mode { Select, Connect };

	explicit FlowsheetScene(SimulationManager* simMgr, QObject* parent = nullptr);

	void setMode(Mode mode);
	Mode mode() const { return m_mode; }

	SimulationManager* simulationManager() const { return m_simMgr; }

	UnitOperationItem* addUnitOperation(const QString& name, const QString& typeName, const QStringList& inPorts, const QStringList& outPorts);
	void removeUnitOperation(UnitOperationItem* item);
	void connectPorts(PortItem* from, PortItem* to);
	void clearFlowsheet();
	void startConnection(PortItem* outputPort);
	void startTemporaryConnection(PortItem* outputPort);
	void cancelConnection();
	StreamItem* addStream(const QPointF& pos);
	StreamBlockItem* addStreamBlock(const QPointF& pos, const std::wstring& streamName = L"");
	void removeStreamBlock(StreamBlockItem* block);

	QList<UnitOperationItem*> unitItems() const;

signals:
	void flowsheetChanged();
	void portConnectionRequested(PortItem* from, PortItem* to);
	void statusMessage(const QString& msg);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
	void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
	void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
	SimulationManager* m_simMgr;
	Mode m_mode = Mode::Select;
	bool m_temporaryConnect = false;

	PortItem* m_pendingPort = nullptr;
	QGraphicsLineItem* m_dragLine = nullptr;

	QMap<QString, UnitOperationItem*> m_unitItems;
	QList<StreamItem*> m_streamItems;
	QList<StreamBlockItem*> m_streamBlocks;
	int m_streamCounter = 0;
};