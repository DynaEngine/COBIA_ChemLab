#pragma once

#include <QGraphicsView>

class FlowsheetScene;
class SimulationManager;

class FlowsheetGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit FlowsheetGraphicsView(FlowsheetScene* scene, SimulationManager* simMgr, QWidget* parent = nullptr);

	void setDragMode(bool enabled);

signals:
	void viewportChanged();

protected:
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
	SimulationManager* m_simMgr = nullptr;
	bool m_panning = false;
	QPoint m_lastPanPos;
};