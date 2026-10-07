#pragma once

#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>

class SimulationManager;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ConfigDialog(SimulationManager* simMgr, QWidget* parent = nullptr);

private slots:
	void onApply();
	void onOk();

private:
	void setupUi();
	void loadSettings();
	void saveSettings();

	SimulationManager* m_simMgr;

	QDoubleSpinBox* m_toleranceSpin;
	QSpinBox* m_maxIterSpin;
	QPushButton* m_applyBtn;
	QPushButton* m_okBtn;
	QPushButton* m_cancelBtn;
};