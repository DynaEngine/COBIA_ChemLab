#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QList>
#include <QString>
#include <QPen>

class PortItem;
class FlowsheetScene;

class UnitOperationItem : public QGraphicsRectItem
{
public:
	explicit UnitOperationItem(const QString& name, const QString& typeName,
		const QStringList& inPorts, const QStringList& outPorts,
		FlowsheetScene* scene, QGraphicsItem* parent = nullptr);

	QString name() const { return m_name; }
	QString typeName() const { return m_typeName; }

	QList<PortItem*> inputPorts() const { return m_inputPorts; }
	QList<PortItem*> outputPorts() const { return m_outputPorts; }
	PortItem* findPort(const QString& portName) const;

	void setHighlighted(bool on);

	enum { Type = QGraphicsItem::UserType + 1 };
	int type() const override { return Type; }

protected:
	UnitOperationItem(const QString& name, const QString& typeName,
		FlowsheetScene* scene, QGraphicsItem* parent);
	virtual void setupAppearance();
	void createPorts(const QStringList& inPorts, const QStringList& outPorts);

	QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

	QString m_name;
	QString m_typeName;
	QGraphicsTextItem* m_label;

	QList<PortItem*> m_inputPorts;
	QList<PortItem*> m_outputPorts;

	static constexpr qreal WIDTH = 100.0;
    static constexpr qreal HEIGHT = 100.0;
	static constexpr qreal PORT_RADIUS = 5.0;
};