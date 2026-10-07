#include "propertyresearchdialog.h"
#include "../simulationmanager.h"
#include "ChemEngine/COBIA/PropertyPackageManager.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeMaterialObject.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QDoubleSpinBox>
#include <cmath>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>

static const char* kDarkLineEdit = R"(
	QLineEdit {
		background: #181825;
		color: #cdd6f4;
		border: 1px solid #45475a;
		border-radius: 4px;
		padding: 4px 8px;
		font-size: 12px;
	}
	QLineEdit:focus {
		border-color: #4a9eff;
	}
)";

static const char* kDarkCombo = R"(
	QComboBox {
		background: #181825;
		color: #cdd6f4;
		border: 1px solid #45475a;
		border-radius: 4px;
		padding: 4px 8px;
		font-size: 12px;
	}
	QComboBox:hover { border-color: #585b70; }
	QComboBox::drop-down {
		border: none;
		width: 20px;
	}
	QComboBox QAbstractItemView {
		background: #181825;
		color: #cdd6f4;
		selection-background-color: #313244;
		border: 1px solid #45475a;
	}
)";

static const char* kPrimaryBtn = R"(
	QPushButton {
		background: #4a9eff;
		color: #1e1e2e;
		border: none;
		border-radius: 4px;
		padding: 6px 18px;
		font-size: 12px;
		font-weight: 600;
	}
	QPushButton:hover { background: #6bb5ff; }
	QPushButton:pressed { background: #3a8eef; }
	QPushButton:disabled { background: #252536; color: #585b70; }
)";

static const char* kSecondaryBtn = R"(
	QPushButton {
		background: #313244;
		color: #cdd6f4;
		border: 1px solid #45475a;
		border-radius: 4px;
		padding: 6px 18px;
		font-size: 12px;
	}
	QPushButton:hover { background: #45475a; border-color: #585b70; }
	QPushButton:pressed { background: #585b70; }
)";

static const char* kSectionLabel = R"(
	font-weight: 600; font-size: 12px; color: #a6adc8; padding-bottom: 2px;
	padding-top: 6px;
)";

PropertyResearchDialog::PropertyResearchDialog(SimulationManager* simMgr, QWidget* parent)
	: QDialog(parent)
	, m_simMgr(simMgr)
{
	setupUi();
}

void PropertyResearchDialog::setActivePackage(const std::wstring& progId)
{
	m_activeProgId = progId;

	m_simMgr->loadPropertyPackage(progId);

	refreshCompounds();
}

void PropertyResearchDialog::setupUi()
{
	setWindowTitle(QStringLiteral("Property Research"));
	setMinimumSize(820, 600);
	resize(860, 680);

	setStyleSheet(QStringLiteral("PropertyResearchDialog { background: #1e1e2e; }"));

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(16, 16, 16, 16);
	mainLayout->setSpacing(12);

	auto* titleLabel = new QLabel(QStringLiteral("Property Research"), this);
	titleLabel->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: 600; color: #cdd6f4;"));
	mainLayout->addWidget(titleLabel);

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setStyleSheet(QStringLiteral(R"(
		QTabWidget::pane {
			background: #1e1e2e;
			border: 1px solid #2d2d3f;
			border-radius: 6px;
		}
		QTabBar::tab {
			background: #181825;
			color: #a6adc8;
			padding: 7px 20px;
			border: none;
			border-bottom: 2px solid transparent;
			font-size: 12px;
		}
		QTabBar::tab:hover { color: #cdd6f4; }
		QTabBar::tab:selected {
			color: #cdd6f4;
			border-bottom: 2px solid #4a9eff;
			font-weight: 600;
		}
	)"));

	m_tabWidget->addTab(createSinglePointTab(), QStringLiteral("Single Point"));
	m_tabWidget->addTab(createRangePlotTab(), QStringLiteral("Range Plot"));
	mainLayout->addWidget(m_tabWidget, 1);

	auto* bottomLayout = new QHBoxLayout();
	bottomLayout->addStretch();
	auto* closeBtn = new QPushButton(QStringLiteral("Close"), this);
	closeBtn->setStyleSheet(QLatin1String(kSecondaryBtn));
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
	bottomLayout->addWidget(closeBtn);
	mainLayout->addLayout(bottomLayout);
}

QWidget* PropertyResearchDialog::createSinglePointTab()
{
	auto* tab = new QWidget(this);
	auto* layout = new QVBoxLayout(tab);
	layout->setContentsMargins(12, 12, 12, 12);
	layout->setSpacing(8);

	auto* compoundLabel = new QLabel(QStringLiteral("Compounds"), this);
	compoundLabel->setStyleSheet(QLatin1String(kSectionLabel));
	layout->addWidget(compoundLabel);

	m_compoundTree = new QTreeWidget(this);
	m_compoundTree->setHeaderLabels({ QStringLiteral("Compound"), QStringLiteral("Ratio") });
	m_compoundTree->setIndentation(0);
	m_compoundTree->setMaximumHeight(160);
	m_compoundTree->setStyleSheet(QStringLiteral(R"(
		QTreeWidget {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			border-radius: 4px;
			font-size: 11px;
		}
		QTreeWidget::item { padding: 3px 6px; }
		QTreeWidget::item:hover { background: #252536; }
		QTreeWidget::item:selected { background: #313244; }
		QHeaderView::section {
			background: #11111b;
			color: #a6adc8;
			border: none;
			border-bottom: 1px solid #2d2d3f;
			padding: 4px 8px;
			font-size: 11px;
		}
	)"));
	layout->addWidget(m_compoundTree);

	auto* tpLayout = new QHBoxLayout();
	tpLayout->setSpacing(16);

	auto* tpForm = new QFormLayout();
	m_tEdit = new QLineEdit(QStringLiteral("298.15"), this);
	m_tEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_tEdit->setMaximumWidth(140);
	m_pEdit = new QLineEdit(QStringLiteral("101325"), this);
	m_pEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_pEdit->setMaximumWidth(140);
	tpForm->addRow(QStringLiteral("T (K):"), m_tEdit);
	tpForm->addRow(QStringLiteral("P (Pa):"), m_pEdit);
	tpLayout->addLayout(tpForm);

	auto* calcBtn = new QPushButton(QStringLiteral("Calculate"), this);
	calcBtn->setStyleSheet(QLatin1String(kPrimaryBtn));
	calcBtn->setFixedHeight(52);
	connect(calcBtn, &QPushButton::clicked, this, &PropertyResearchDialog::onSinglePointCalc);
	tpLayout->addWidget(calcBtn);
	tpLayout->addStretch();

	layout->addLayout(tpLayout);

	auto* resultLabel = new QLabel(QStringLiteral("Results"), this);
	resultLabel->setStyleSheet(QLatin1String(kSectionLabel));
	layout->addWidget(resultLabel);

	m_resultTable = new QTableWidget(0, 3, this);
	m_resultTable->setHorizontalHeaderLabels({
		QStringLiteral("Property"), QStringLiteral("Phase"), QStringLiteral("Value") });
	m_resultTable->horizontalHeader()->setStretchLastSection(true);
	m_resultTable->verticalHeader()->setVisible(false);
	m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_resultTable->setStyleSheet(QStringLiteral(R"(
		QTableWidget {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			border-radius: 4px;
			gridline-color: #2d2d3f;
			font-size: 11px;
			selection-background-color: #313244;
		}
		QTableWidget::item { padding: 4px 8px; }
		QHeaderView::section {
			background: #11111b;
			color: #a6adc8;
			border: none;
			border-bottom: 2px solid #313244;
			padding: 6px 10px;
			font-size: 11px;
			font-weight: 600;
		}
	)"));
	layout->addWidget(m_resultTable, 1);

	return tab;
}

QWidget* PropertyResearchDialog::createRangePlotTab()
{
	auto* tab = new QWidget(this);
	auto* layout = new QVBoxLayout(tab);
	layout->setContentsMargins(12, 12, 12, 12);
	layout->setSpacing(8);

	auto* settingsLayout = new QHBoxLayout();

	auto* leftForm = new QFormLayout();
	leftForm->setSpacing(8);

	m_propCombo = new QComboBox(this);
	m_propCombo->setStyleSheet(QLatin1String(kDarkCombo));
	m_propCombo->addItem(QStringLiteral("Enthalpy (J/mol)"), QVariant(QStringLiteral("enthalpy")));
	m_propCombo->addItem(QStringLiteral("Density (kg/m3)"), QVariant(QStringLiteral("density")));
	m_propCombo->addItem(QStringLiteral("Entropy (J/(mol·K))"), QVariant(QStringLiteral("entropy")));
	m_propCombo->addItem(QStringLiteral("Heat of Vap. (J/mol)"), QVariant(QStringLiteral("heatOfVaporization")));
	m_propCombo->addItem(QStringLiteral("Surface Tension (N/m)"), QVariant(QStringLiteral("surfaceTension")));
	m_propCombo->addItem(QStringLiteral("Viscosity (Pa·s)"), QVariant(QStringLiteral("viscosity")));
	leftForm->addRow(QStringLiteral("Property:"), m_propCombo);

	m_varCombo = new QComboBox(this);
	m_varCombo->setStyleSheet(QLatin1String(kDarkCombo));
	m_varCombo->addItem(QStringLiteral("Temperature (K)"), QVariant(QStringLiteral("T")));
	m_varCombo->addItem(QStringLiteral("Pressure (Pa)"), QVariant(QStringLiteral("P")));
	connect(m_varCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
		bool isT = (m_varCombo->currentData().toString() == QStringLiteral("T"));
		m_fixedValLabel->setText(isT ? QStringLiteral("Fixed P (Pa):") : QStringLiteral("Fixed T (K):"));
		m_fixedValEdit->setText(isT ? QStringLiteral("101325") : QStringLiteral("298.15"));
	});
	leftForm->addRow(QStringLiteral("Variable:"), m_varCombo);

	m_fixedValLabel = new QLabel(QStringLiteral("Fixed P (Pa):"), this);
	m_fixedValLabel->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 11px;"));
	m_fixedValEdit = new QLineEdit(QStringLiteral("101325"), this);
	m_fixedValEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_fixedValEdit->setMaximumWidth(120);
	leftForm->addRow(m_fixedValLabel, m_fixedValEdit);

	settingsLayout->addLayout(leftForm);

	auto* rightForm = new QFormLayout();
	rightForm->setSpacing(8);

	m_rangeMinEdit = new QLineEdit(QStringLiteral("273.15"), this);
	m_rangeMinEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_rangeMinEdit->setMaximumWidth(120);
	m_rangeMaxEdit = new QLineEdit(QStringLiteral("373.15"), this);
	m_rangeMaxEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_rangeMaxEdit->setMaximumWidth(120);
	m_rangeStepEdit = new QLineEdit(QStringLiteral("5"), this);
	m_rangeStepEdit->setStyleSheet(QLatin1String(kDarkLineEdit));
	m_rangeStepEdit->setMaximumWidth(120);

	rightForm->addRow(QStringLiteral("Min:"), m_rangeMinEdit);
	rightForm->addRow(QStringLiteral("Max:"), m_rangeMaxEdit);
	rightForm->addRow(QStringLiteral("Step:"), m_rangeStepEdit);

	settingsLayout->addLayout(rightForm);

	auto* plotBtn = new QPushButton(QStringLiteral("Plot"), this);
	plotBtn->setStyleSheet(QLatin1String(kPrimaryBtn));
	plotBtn->setFixedHeight(60);
	connect(plotBtn, &QPushButton::clicked, this, &PropertyResearchDialog::onRangePlot);
	settingsLayout->addWidget(plotBtn);

	settingsLayout->addStretch();
	layout->addLayout(settingsLayout);

	auto* splitter = new QSplitter(Qt::Vertical, this);

	m_rangeTable = new QTableWidget(0, 3, this);
	m_rangeTable->setHorizontalHeaderLabels({
		QStringLiteral("X"), QStringLiteral("Y"), QStringLiteral("Phase") });
	m_rangeTable->horizontalHeader()->setStretchLastSection(true);
	m_rangeTable->verticalHeader()->setVisible(false);
	m_rangeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_rangeTable->setMaximumHeight(140);
	m_rangeTable->setStyleSheet(QStringLiteral(R"(
		QTableWidget {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			border-radius: 4px;
			gridline-color: #2d2d3f;
			font-size: 11px;
		}
		QTableWidget::item { padding: 3px 6px; }
		QHeaderView::section {
			background: #11111b;
			color: #a6adc8;
			border: none;
			border-bottom: 1px solid #2d2d3f;
			padding: 4px 8px;
			font-size: 11px;
		}
	)"));
	splitter->addWidget(m_rangeTable);

	m_plotContainer = new QWidget(this);
	splitter->addWidget(m_plotContainer);

	layout->addWidget(splitter, 1);

	return tab;
}

void PropertyResearchDialog::refreshCompounds()
{
	m_compoundTree->clear();
	qDeleteAll(m_ratioSpinners);
	m_ratioSpinners.clear();

	auto* ppMgr = m_simMgr ? m_simMgr->propertyPackageManager() : nullptr;
	if (!ppMgr)
	{
		auto* empty = new QTreeWidgetItem(m_compoundTree,
			{ QStringLiteral("(No Property Package available)") });
		empty->setFlags(Qt::NoItemFlags);
		return;
	}

	const auto& compounds = ppMgr->getCompoundsFromPackage();
	if (compounds.empty())
	{
		auto* empty = new QTreeWidgetItem(m_compoundTree,
			{ QStringLiteral("(No compounds - load a Property Package first)") });
		empty->setFlags(Qt::NoItemFlags);
		return;
	}

	double defaultRatio = 1.0 / static_cast<double>(compounds.size());

	for (size_t i = 0; i < compounds.size(); ++i)
	{
		auto* item = new QTreeWidgetItem(m_compoundTree);
		const auto& name = compounds[i].getConstantProperties().name;
		item->setText(0, QString::fromStdWString(name));
		item->setData(0, Qt::UserRole, static_cast<int>(i));
		item->setToolTip(0, QString::fromStdWString(name));

		auto* spin = new QDoubleSpinBox(m_compoundTree);
		spin->setRange(0.0, 1.0);
		spin->setSingleStep(0.05);
		spin->setDecimals(4);
		spin->setValue(defaultRatio);
		spin->setStyleSheet(QStringLiteral(R"(
			QDoubleSpinBox {
				background: #181825;
				color: #cdd6f4;
				border: 1px solid #45475a;
				border-radius: 3px;
				font-size: 11px;
				padding: 2px 4px;
			}
			QDoubleSpinBox:focus { border-color: #4a9eff; }
		)"));
		m_compoundTree->setItemWidget(item, 1, spin);
		m_ratioSpinners.push_back(spin);
	}

	m_compoundTree->resizeColumnToContents(0);
}

void PropertyResearchDialog::onSinglePointCalc()
{
	m_resultTable->setRowCount(0);

	if (!ensureMaterialReady())
	{
		QMessageBox::warning(this,
			QStringLiteral("No Data"),
			QStringLiteral("Please load a Property Package first, then select compounds."));
		return;
	}

	bool okT, okP;
	double T = m_tEdit->text().toDouble(&okT);
	double P = m_pEdit->text().toDouble(&okP);
	if (!okT || !okP)
	{
		QMessageBox::warning(this,
			QStringLiteral("Invalid Input"),
			QStringLiteral("Please enter valid numbers for T and P."));
		return;
	}

	auto* ppMgr = m_simMgr->propertyPackageManager();
	auto* matObj = ppMgr ? ppMgr->getMaterialObject() : nullptr;

	m_material->setTemperature(T);
	m_material->setPressure(P);
	if (matObj)
	{
		matObj->setOverallTemperature(T);
		matObj->setOverallPressure(P);
	}

	std::vector<double> fractions;
	for (auto* spin : m_ratioSpinners)
		fractions.push_back(spin->value());
	if (!fractions.empty() && matObj)
		matObj->setOverallComposition(fractions);

	if (ppMgr && matObj)
	{
		ppMgr->setMaterial(m_material.get());
		if (!ppMgr->calculateEquilibrium())
		{
			QMessageBox::warning(this,
				QStringLiteral("Equilibrium Failed"),
				QStringLiteral("The Property Package could not perform the equilibrium calculation.\n"
					"Check that T, P and composition are valid."));
			return;
		}

		std::vector<std::wstring> calcProps = {
			L"density", L"enthalpy", L"entropy",
			L"heatOfVaporization", L"surfaceTension", L"viscosity"
		};
		const auto& phases = matObj->getPhaseLabels();
		if (phases.empty())
		{
			ppMgr->calculateProperties(calcProps, L"Overall");
		}
		else
		{
			for (const auto& ph : phases)
				ppMgr->calculateProperties(calcProps, ph);
		}

		m_material->setCapeMaterialObject(matObj);
	}

	m_material->flashTP();

	populateResultsTable();
}

bool PropertyResearchDialog::ensureMaterialReady()
{
	auto* ppMgr = m_simMgr ? m_simMgr->propertyPackageManager() : nullptr;

	if (!ppMgr) return false;

	const auto& compounds = ppMgr->getCompoundsFromPackage();
	if (compounds.empty()) return false;

	m_material = std::make_unique<ChemEngine::COBIA::CapeMaterialStream>();
	::COBIA::CapeStringImpl matName(L"ResearchCalc");
	m_material->createMaterial(matName);
	m_material->addCompounds(compounds);

	ppMgr->createMaterialObject();
	ppMgr->setMaterialCompounds(compounds);

	return true;
}

void PropertyResearchDialog::populateResultsTable()
{
	auto addRow = [this](int& r, const QString& prop, const QString& phase, double val) {
		m_resultTable->insertRow(r);
		m_resultTable->setItem(r, 0, new QTableWidgetItem(prop));
		m_resultTable->setItem(r, 1, new QTableWidgetItem(phase));
		m_resultTable->setItem(r, 2, new QTableWidgetItem(QString::number(val, 'g', 4)));
		++r;
	};

	if (!m_material) return;

	QString phase = QStringLiteral("Overall");

	auto* ppMgr = m_simMgr->propertyPackageManager();
	auto* matObj = ppMgr ? ppMgr->getMaterialObject() : nullptr;
	if (matObj)
	{
		const auto& phases = matObj->getPhaseLabels();
		if (!phases.empty())
			phase = QString::fromStdWString(phases[0]);
	}

	int r = 0;

	double T = m_material->getTemperature();
	double P = m_material->getPressure();
	addRow(r, QStringLiteral("Temperature"), phase, T);
	addRow(r, QStringLiteral("Pressure"), phase, P);

	if (matObj)
	{
		const auto& phases = matObj->getPhaseLabels();
		std::wstring primaryPhase = phases.empty() ? L"Overall" : phases[0];

		struct PropEntry { const wchar_t* prop; const char* label; };
		static const PropEntry props[] = {
			{L"density", "Density (kg/m3)"},
			{L"enthalpy", "Enthalpy (J/mol)"},
			{L"entropy", "Entropy (J/(mol·K))"},
			{L"heatOfVaporization", "Heat of Vap. (J/mol)"},
			{L"surfaceTension", "Surface Tension (N/m)"},
			{L"viscosity", "Viscosity (Pa·s)"},
		};

		for (auto& p : props)
		{
			std::wstring propW(p.prop);
			double v = matObj->getCachedOverallProp(propW);
			if (v == 0.0 && propW != L"temperature" && propW != L"pressure")
			{
				v = matObj->getCachedSinglePhaseProp(propW, primaryPhase);
				if (v == 0.0 && primaryPhase != L"Overall")
					v = matObj->getCachedSinglePhaseProp(propW, L"Overall");
				if (v == 0.0)
				{
					for (const auto& ph : phases)
					{
						v = matObj->getCachedSinglePhaseProp(propW, ph);
						if (v != 0.0) break;
					}
				}
			}
			if (propW == L"density" && v > 0.0)
			{
				double mw = matObj->getMolecularWeight();
				if (mw > 0.0)
					v = v * mw * 1e-3;
			}
			addRow(r, QLatin1String(p.label), phase, v);
		}
	}
	else
	{
		struct PropEntry { const wchar_t* prop; const char* label; };
		static const PropEntry props[] = {
			{L"density", "Density (kg/m3)"},
			{L"enthalpy", "Enthalpy (J/mol)"},
			{L"entropy", "Entropy (J/(mol·K))"},
			{L"heatOfVaporization", "Heat of Vap. (J/mol)"},
			{L"surfaceTension", "Surface Tension (N/m)"},
			{L"viscosity", "Viscosity (Pa·s)"},
		};

		for (auto& p : props)
		{
			double v = m_material->getOverallProp(std::wstring(p.prop), L"mole");
			addRow(r, QLatin1String(p.label), phase, v);
		}
	}
}

void PropertyResearchDialog::onRangePlot()
{
	if (!ensureMaterialReady())
	{
		QMessageBox::warning(this,
			QStringLiteral("No Data"),
			QStringLiteral("Please load a Property Package in the library dialog first."));
		return;
	}

	bool okMin, okMax, okStep, okFixed;
	double xMin = m_rangeMinEdit->text().toDouble(&okMin);
	double xMax = m_rangeMaxEdit->text().toDouble(&okMax);
	double xStep = m_rangeStepEdit->text().toDouble(&okStep);
	double fixedVal = m_fixedValEdit->text().toDouble(&okFixed);
	if (!okMin || !okMax || !okStep || !okFixed || xStep <= 0)
	{
		QMessageBox::warning(this,
			QStringLiteral("Invalid Input"),
			QStringLiteral("Please enter valid numeric values."));
		return;
	}

	bool varyT = (m_varCombo->currentData().toString() == QStringLiteral("T"));
	QString propKey = m_propCombo->currentData().toString();
	std::wstring propW = propKey.toStdWString();

	auto* ppMgr = m_simMgr->propertyPackageManager();
	auto* matObj = ppMgr ? ppMgr->getMaterialObject() : nullptr;

	std::vector<double> fractions;
	for (auto* spin : m_ratioSpinners)
		fractions.push_back(spin->value());
	if (!fractions.empty() && matObj)
		matObj->setOverallComposition(fractions);

	m_rangeTable->setRowCount(0);

	std::vector<std::wstring> calcProps = { propW };

	for (double x = xMin; x <= xMax + xStep * 0.1; x += xStep)
	{
		double T = varyT ? x : fixedVal;
		double P = varyT ? fixedVal : x;

		m_material->setTemperature(T);
		m_material->setPressure(P);
		if (matObj)
		{
			matObj->setOverallTemperature(T);
			matObj->setOverallPressure(P);
		}

		double y = 0.0;
		QString phaseStr = QStringLiteral("Overall");

		if (ppMgr && matObj)
		{
			ppMgr->setMaterial(m_material.get());
			if (ppMgr->calculateEquilibrium())
			{
				const auto& phases = matObj->getPhaseLabels();
				if (!phases.empty())
				{
					phaseStr = QString::fromStdWString(phases[0]);
					ppMgr->calculateProperties(calcProps, phases[0]);
				}
				else
				{
					ppMgr->calculateProperties(calcProps, L"Overall");
				}
				m_material->setCapeMaterialObject(matObj);

				std::wstring primaryPhase = phases.empty() ? L"Overall" : phases[0];
				y = matObj->getCachedOverallProp(propW);
				if (y == 0.0 && propW != L"temperature" && propW != L"pressure")
				{
					y = matObj->getCachedSinglePhaseProp(propW, primaryPhase);
					if (y == 0.0 && primaryPhase != L"Overall")
						y = matObj->getCachedSinglePhaseProp(propW, L"Overall");
					if (y == 0.0)
					{
						for (const auto& ph : phases)
						{
							y = matObj->getCachedSinglePhaseProp(propW, ph);
							if (y != 0.0) break;
						}
					}
				}
				if (propW == L"density" && y > 0.0)
				{
					double mw = matObj->getMolecularWeight();
					if (mw > 0.0)
						y = y * mw * 1e-3;
				}
			}
			else
			{
				m_material->flashTP();
				y = m_material->getOverallProp(propW, L"mole");
			}
		}
		else
		{
			m_material->flashTP();
			y = m_material->getOverallProp(propW, L"mole");
		}

		int row = m_rangeTable->rowCount();
		m_rangeTable->insertRow(row);
		m_rangeTable->setItem(row, 0, new QTableWidgetItem(QString::number(x, 'f', 1)));
		m_rangeTable->setItem(row, 1, new QTableWidgetItem(QString::number(y, 'g', 4)));
		m_rangeTable->setItem(row, 2, new QTableWidgetItem(phaseStr));
	}

	drawPlot();
}

void PropertyResearchDialog::drawPlot()
{
	auto* layout = m_plotContainer->layout();
	if (layout)
	{
		QLayoutItem* child;
		while ((child = layout->takeAt(0)) != nullptr)
		{
			delete child->widget();
			delete child;
		}
	}
	else
	{
		layout = new QVBoxLayout(m_plotContainer);
		layout->setContentsMargins(0, 0, 0, 0);
	}

	QByteArray propDisplay = m_propCombo->currentText().toUtf8();
	QByteArray varDisplay = m_varCombo->currentText().toUtf8();

	QwtPlot* plot = new QwtPlot(m_plotContainer);
	plot->setCanvasBackground(QColor(0x1e, 0x1e, 0x2e));
	plot->setTitle(QStringLiteral("%1 vs %2").arg(QString::fromUtf8(propDisplay), QString::fromUtf8(varDisplay)));
	plot->setAxisTitle(QwtPlot::xBottom, m_varCombo->currentText());
	plot->setAxisTitle(QwtPlot::yLeft, m_propCombo->currentText());
	plot->setMinimumHeight(250);

	plot->setStyleSheet(QStringLiteral(R"(
		QwtPlot {
			background: #1e1e2e;
			border: 1px solid #2d2d3f;
			border-radius: 6px;
		}
		QwtPlotCanvas {
			background: #1e1e2e;
		}
	)"));

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid->setPen(QColor(0x45, 0x47, 0x5a), 0.5);
	grid->attach(plot);

	QVector<QPointF> pts;
	for (int i = 0; i < m_rangeTable->rowCount(); ++i)
	{
		double x = m_rangeTable->item(i, 0)->text().toDouble();
		double y = m_rangeTable->item(i, 1)->text().toDouble();
		pts.append(QPointF(x, y));
	}

	QwtPlotCurve* curve = new QwtPlotCurve();
	curve->setSamples(pts);
	curve->setPen(QColor(0x4a, 0x9e, 0xff), 2.0);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->attach(plot);

	plot->replot();
	layout->addWidget(plot);
}