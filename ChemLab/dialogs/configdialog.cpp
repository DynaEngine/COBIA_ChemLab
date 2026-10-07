#include "configdialog.h"
#include "../simulationmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>

ConfigDialog::ConfigDialog(SimulationManager* simMgr, QWidget* parent)
	: QDialog(parent)
	, m_simMgr(simMgr)
{
	setupUi();
	loadSettings();
}

void ConfigDialog::setupUi()
{
	setWindowTitle(QStringLiteral("Configuration"));
	setMinimumSize(420, 280);
	resize(440, 300);

	setStyleSheet(QStringLiteral(R"(
		ConfigDialog {
			background: #1e1e2e;
		}
		QLabel {
			color: #cdd6f4;
			font-size: 11px;
		}
		QGroupBox {
			color: #cdd6f4;
			font-size: 12px;
			font-weight: bold;
			border: 1px solid #45475a;
			border-radius: 6px;
			margin-top: 12px;
			padding-top: 14px;
		}
		QGroupBox::title {
			subcontrol-origin: margin;
			left: 12px;
			padding: 0 6px;
		}
		QDoubleSpinBox, QSpinBox {
			background: #313244;
			color: #cdd6f4;
			border: 1px solid #45475a;
			border-radius: 4px;
			padding: 4px 8px;
			font-size: 11px;
			min-width: 100px;
		}
		QDoubleSpinBox:focus, QSpinBox:focus {
			border-color: #4a9eff;
		}
		QPushButton {
			background: #313244;
			color: #cdd6f4;
			border: 1px solid #45475a;
			border-radius: 4px;
			padding: 6px 18px;
			font-size: 11px;
		}
		QPushButton:hover {
			background: #45475a;
			border-color: #585b70;
		}
		QPushButton#okBtn {
			background: #4a9eff;
			color: #1e1e2e;
			border: none;
			font-weight: bold;
		}
		QPushButton#okBtn:hover {
			background: #6db5ff;
		}
	)"));

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(20, 20, 20, 20);
	mainLayout->setSpacing(16);

	auto* titleLabel = new QLabel(QStringLiteral("Solver Configuration"), this);
	titleLabel->setStyleSheet(QStringLiteral(
		"font-size: 16px; font-weight: 600; color: #cdd6f4; padding-bottom: 4px;"));
	mainLayout->addWidget(titleLabel);

	auto* solverGroup = new QGroupBox(QStringLiteral(" Solver Parameters "), this);
	auto* formLayout = new QFormLayout(solverGroup);
	formLayout->setSpacing(10);
	formLayout->setContentsMargins(16, 20, 16, 16);

	m_toleranceSpin = new QDoubleSpinBox(this);
	m_toleranceSpin->setDecimals(10);
	m_toleranceSpin->setRange(1e-15, 1.0);
	m_toleranceSpin->setSingleStep(1e-6);
	m_toleranceSpin->setToolTip(QStringLiteral("Convergence tolerance for tear streams and recycle loops"));

	auto* tolLabel = new QLabel(QStringLiteral("Tolerance:"), this);
	tolLabel->setToolTip(m_toleranceSpin->toolTip());
	formLayout->addRow(tolLabel, m_toleranceSpin);

	m_maxIterSpin = new QSpinBox(this);
	m_maxIterSpin->setRange(1, 100000);
	m_maxIterSpin->setSingleStep(10);
	m_maxIterSpin->setToolTip(QStringLiteral("Maximum number of solver iterations before giving up"));

	auto* iterLabel = new QLabel(QStringLiteral("Max Iterations:"), this);
	iterLabel->setToolTip(m_maxIterSpin->toolTip());
	formLayout->addRow(iterLabel, m_maxIterSpin);

	mainLayout->addWidget(solverGroup);

	mainLayout->addStretch();

	auto* btnLayout = new QHBoxLayout();
	btnLayout->setSpacing(10);
	btnLayout->addStretch();

	m_applyBtn = new QPushButton(QStringLiteral("Apply"), this);
	connect(m_applyBtn, &QPushButton::clicked, this, &ConfigDialog::onApply);
	btnLayout->addWidget(m_applyBtn);

	m_cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
	connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	btnLayout->addWidget(m_cancelBtn);

	m_okBtn = new QPushButton(QStringLiteral("OK"), this);
	m_okBtn->setObjectName(QStringLiteral("okBtn"));
	connect(m_okBtn, &QPushButton::clicked, this, &ConfigDialog::onOk);
	btnLayout->addWidget(m_okBtn);

	mainLayout->addLayout(btnLayout);
}

void ConfigDialog::loadSettings()
{
	m_toleranceSpin->setValue(m_simMgr->tolerance());
	m_maxIterSpin->setValue(m_simMgr->maxIterations());
}

void ConfigDialog::saveSettings()
{
	m_simMgr->setTolerance(m_toleranceSpin->value());
	m_simMgr->setMaxIterations(m_maxIterSpin->value());
}

void ConfigDialog::onApply()
{
	saveSettings();
}

void ConfigDialog::onOk()
{
	saveSettings();
	accept();
}