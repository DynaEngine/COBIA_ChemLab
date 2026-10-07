#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QList>

#include "ChemEngine/COBIA/CapeMaterialStream.h"

class SimulationManager;
class QwtPlot;

class PropertyResearchDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyResearchDialog(SimulationManager* simMgr, QWidget* parent = nullptr);

	void setActivePackage(const std::wstring& progId);

private slots:
	void onSinglePointCalc();
	void onRangePlot();

private:
	void setupUi();
	QWidget* createSinglePointTab();
	QWidget* createRangePlotTab();

	void refreshCompounds();
	void populateResultsTable();
	void drawPlot();

	bool ensureMaterialReady();

	SimulationManager* m_simMgr = nullptr;
	std::wstring m_activeProgId;

	QTabWidget* m_tabWidget = nullptr;

	QTreeWidget* m_compoundTree = nullptr;
	QList<QDoubleSpinBox*> m_ratioSpinners;

	QLineEdit* m_tEdit = nullptr;
	QLineEdit* m_pEdit = nullptr;
	QTableWidget* m_resultTable = nullptr;
	QLabel* m_statusLabel = nullptr;

	QComboBox* m_propCombo = nullptr;
	QComboBox* m_varCombo = nullptr;
	QLineEdit* m_rangeMinEdit = nullptr;
	QLineEdit* m_rangeMaxEdit = nullptr;
	QLineEdit* m_rangeStepEdit = nullptr;
	QLineEdit* m_fixedValEdit = nullptr;
	QLabel* m_fixedValLabel = nullptr;
	QWidget* m_plotContainer = nullptr;
	QTableWidget* m_rangeTable = nullptr;

	std::unique_ptr<ChemEngine::COBIA::CapeMaterialStream> m_material;
};