#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class SimulationManager;

class UnitTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit UnitTreeWidget(QWidget* parent = nullptr) : QTreeWidget(parent) {}

protected:
	QStringList mimeTypes() const override
	{
		return { QStringLiteral("application/x-chemlab-unit") };
	}

	QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const override
	{
		if (items.isEmpty()) return nullptr;

		auto* item = items.first();
		QString name = item->data(0, Qt::UserRole + 1).toString();
		QString typeName = item->data(0, Qt::UserRole + 5).toString();
		QString inPortsStr = item->data(0, Qt::UserRole + 6).toString();
		QString outPortsStr = item->data(0, Qt::UserRole + 7).toString();
		QString progId = item->data(0, Qt::UserRole).toString();
		QString category = item->data(0, Qt::UserRole).toString();

		if (name.isEmpty() || typeName.isEmpty()) return nullptr;

		QJsonObject obj;
		obj["name"] = name;
		obj["type"] = typeName;
		obj["category"] = category;
		if (!progId.isEmpty() && category != QStringLiteral("builtin"))
			obj["progId"] = progId;

		QJsonArray inArr, outArr;
		for (const auto& s : inPortsStr.split('|', Qt::SkipEmptyParts))
			inArr.append(s.trimmed());
		for (const auto& s : outPortsStr.split('|', Qt::SkipEmptyParts))
			outArr.append(s.trimmed());
		obj["inPorts"] = inArr;
		obj["outPorts"] = outArr;

		auto* data = new QMimeData();
		data->setData(QStringLiteral("application/x-chemlab-unit"),
			QJsonDocument(obj).toJson(QJsonDocument::Compact));
		return data;
	}
};

class UnitOperationPanel : public QWidget
{
	Q_OBJECT

public:
	explicit UnitOperationPanel(SimulationManager* simMgr, QWidget* parent = nullptr);

	void refreshUnitList();

signals:
	void unitSelected(const QString& name, const QString& progId);
	void unitDoubleClicked(const QString& name, const QString& progId);

private slots:
	void onItemClicked(QTreeWidgetItem* item, int col);
	void onItemDoubleClicked(QTreeWidgetItem* item, int col);

private:
	void setupUi();
	void populateTree();
	void populateFlatList(bool show);
	void addBuiltInSection(QTreeWidgetItem* parent);
	void addCOBIASection(QTreeWidgetItem* parent);

	SimulationManager* m_simMgr;
	UnitTreeWidget* m_tree;
	QPushButton* m_refreshBtn;
	QToolButton* m_viewToggle;
	QLabel* m_statusLabel;
	bool m_flatView = false;
};