#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "ChemEngine/COBIA/UnitOperationManager.h"

namespace ChemEngine { namespace COBIA { class CapeMaterialStream; } }

class SimulationManager;

class UnitInfoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit UnitInfoDialog(SimulationManager* simMgr, QWidget* parent = nullptr);

	void loadUnitInfo(const ChemEngine::COBIA::UnitOperationInfo& info);
	void loadBuiltInUnitInfo(const std::wstring& unitName);
	void loadStreamResults(ChemEngine::COBIA::CapeMaterialStream& stream);
	void loadFeedStreamEditor(ChemEngine::COBIA::CapeMaterialStream& stream);

private:
	void setupUi();
	void populatePortList();
	void populateParamTree();

	void setupStreamTreeView();
	void clearStreamContent();
	void populateStreamProperties(bool isFeed);
	void refreshCalculatedProperties();
	void applyFeedProperties();

	QTreeWidgetItem* addCategory(const QString& title);
	QTreeWidgetItem* addProperty(QTreeWidgetItem* parent,
		const QString& name, const QString& value, const QString& unit = QString(),
		const QColor& valColor = QColor(0xcd, 0xd6, 0xf4));
	QTreeWidgetItem* addEditableProperty(QTreeWidgetItem* parent,
		const QString& name, double value, double min, double max, int decimals,
		const QString& suffix, const QColor& valColor = QColor(0xa6, 0xe3, 0xa1));
	void addCompositionRows(QTreeWidgetItem* parent, bool editable);

	SimulationManager* m_simMgr;

	QLabel* m_nameLabel;
	QLabel* m_progIdLabel;
	QLabel* m_vendorLabel;
	QLabel* m_typeLabel;
	QLabel* m_descLabel;
	QLabel* m_portCountLabel;
	QLabel* m_paramCountLabel;
	QLabel* m_statusLabel;

	QTableWidget* m_portTable;
	QTableWidget* m_portCompareTable = nullptr;
	QTreeWidget* m_paramTree;
	QPushButton* m_closeBtn;

	QTreeWidget* m_streamTree = nullptr;
	QPushButton* m_applyBtn = nullptr;
	ChemEngine::COBIA::CapeMaterialStream* m_stream = nullptr;
	bool m_isFeed = false;

	QTreeWidgetItem* m_basicCat = nullptr;
	QTreeWidgetItem* m_thermoCat = nullptr;
	QTreeWidgetItem* m_compositionCat = nullptr;
	QTreeWidgetItem* m_phaseCat = nullptr;

	QDoubleSpinBox* m_tempEdit = nullptr;
	QDoubleSpinBox* m_pressEdit = nullptr;
	QDoubleSpinBox* m_flowEdit = nullptr;
	QTreeWidgetItem* m_tempItem = nullptr;
	QTreeWidgetItem* m_pressItem = nullptr;
	QTreeWidgetItem* m_flowItem = nullptr;

	ChemEngine::COBIA::UnitOperationInfo m_unitInfo;
	int m_portCount = 0;
	int m_paramCount = 0;
};