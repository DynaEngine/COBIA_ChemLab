#pragma once

#include <QDialog>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class SimulationManager;
namespace ChemEngine { namespace COBIA { enum class PropertyPackageType; } }

class PropertyPackageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyPackageDialog(SimulationManager* simMgr, QWidget* parent = nullptr);

signals:
	void packageLoaded(const QString& name);

public slots:
	void onRefresh();

private slots:
	void onLoad();
	void onStudy();
	void onFilterChanged(int index);

private:
	void setupUi();
	void populateTable();
	void openStudy();

	SimulationManager* m_simMgr;
	QTableWidget* m_table;
	QComboBox* m_typeFilter = nullptr;
	QPushButton* m_refreshBtn;
	QPushButton* m_loadBtn;
	QPushButton* m_closeBtn;
	QLabel* m_statusLabel;
	QPushButton* m_studyBtn = nullptr;

	int m_filterType = 0;
};