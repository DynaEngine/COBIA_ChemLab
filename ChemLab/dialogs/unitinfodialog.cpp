#include "unitinfodialog.h"
#include "../simulationmanager.h"

#include "ChemEngine/COBIA/UnitOperationManager.h"
#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeMaterialObject.h"
#include "ChemEngine/COBIA/PropertyPackageManager.h"
#include "ChemEngine/Base/CompoundConstantProperties.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"
#include "ChemEngine/PropertyPackages/PropertyPackage.h"
#include "ChemEngine/Units/UnitOperationBase.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QSplitter>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

UnitInfoDialog::UnitInfoDialog(SimulationManager* simMgr, QWidget* parent)
	: QDialog(parent)
	, m_simMgr(simMgr)
	, m_nameLabel(nullptr)
	, m_progIdLabel(nullptr)
	, m_vendorLabel(nullptr)
	, m_typeLabel(nullptr)
	, m_descLabel(nullptr)
	, m_portCountLabel(nullptr)
	, m_paramCountLabel(nullptr)
	, m_statusLabel(nullptr)
	, m_portTable(nullptr)
	, m_paramTree(nullptr)
	, m_closeBtn(nullptr)
{
	setupUi();
}

void UnitInfoDialog::setupUi()
{
	setWindowTitle(QStringLiteral("Unit Operation Info"));
	resize(620, 500);
	setMinimumSize(480, 360);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(12, 12, 12, 12);
	mainLayout->setSpacing(10);

	auto* infoGroup = new QGroupBox(QStringLiteral("Unit Identification"), this);
	infoGroup->setObjectName(QStringLiteral("infoGroup"));
	auto* infoLayout = new QVBoxLayout(infoGroup);
	infoLayout->setSpacing(4);

	m_nameLabel = new QLabel(this);
	m_nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4a9eff;");
	infoLayout->addWidget(m_nameLabel);

	m_typeLabel = new QLabel(this);
	m_typeLabel->setStyleSheet("color: #a6adc8;");
	infoLayout->addWidget(m_typeLabel);

	m_progIdLabel = new QLabel(this);
	m_progIdLabel->setStyleSheet("color: #a6adc8; font-size: 11px;");
	infoLayout->addWidget(m_progIdLabel);

	m_vendorLabel = new QLabel(this);
	m_vendorLabel->setStyleSheet("color: #a6adc8; font-size: 11px;");
	infoLayout->addWidget(m_vendorLabel);

	m_descLabel = new QLabel(this);
	m_descLabel->setWordWrap(true);
	m_descLabel->setStyleSheet("color: #cdd6f4; font-size: 12px; margin-top: 4px;");
	infoLayout->addWidget(m_descLabel);

	m_statusLabel = new QLabel(this);
	m_statusLabel->setStyleSheet("color: #f38ba8; font-size: 11px;");
	infoLayout->addWidget(m_statusLabel);

	mainLayout->addWidget(infoGroup);

	auto* splitter = new QSplitter(Qt::Vertical, this);

	auto* portsGroup = new QGroupBox(QStringLiteral("Ports"), this);
	auto* portsLayout = new QVBoxLayout(portsGroup);

	m_portCountLabel = new QLabel(this);
	m_portCountLabel->setStyleSheet("color: #a6adc8; font-size: 11px; margin-bottom: 2px;");
	portsLayout->addWidget(m_portCountLabel);

	m_portTable = new QTableWidget(0, 4, this);
	m_portTable->setHorizontalHeaderLabels({
		QStringLiteral("Name"),
		QStringLiteral("Direction"),
		QStringLiteral("Type"),
		QStringLiteral("Description") });
	m_portTable->horizontalHeader()->setStretchLastSection(true);
	m_portTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_portTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_portTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	m_portTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_portTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_portTable->setAlternatingRowColors(true);
	m_portTable->verticalHeader()->setVisible(false);
	m_portTable->setStyleSheet(
		"QTableWidget { background:#1e1e2e; alternate-background-color:#252538; gridline-color:#313244; "
		"color:#cdd6f4; border:1px solid #313244; }"
		"QTableWidget::item:selected { background:#45475a; }"
		"QHeaderView::section { background:#313244; color:#a6adc8; border:none; padding:4px; }");
	portsLayout->addWidget(m_portTable);

	m_portCompareTable = new QTableWidget(0, 2, this);
	m_portCompareTable->horizontalHeader()->setStretchLastSection(true);
	m_portCompareTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_portCompareTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_portCompareTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_portCompareTable->setAlternatingRowColors(true);
	m_portCompareTable->verticalHeader()->setVisible(false);
	m_portCompareTable->setStyleSheet(
		"QTableWidget { background:#1e1e2e; alternate-background-color:#252538; gridline-color:#313244; "
		"color:#cdd6f4; border:1px solid #313244; }"
		"QTableWidget::item:selected { background:#45475a; }"
		"QHeaderView::section { background:#313244; color:#a6adc8; border:none; padding:4px; }");
	m_portCompareTable->setVisible(false);
	portsLayout->addWidget(m_portCompareTable);

	splitter->addWidget(portsGroup);

	auto* paramsGroup = new QGroupBox(QStringLiteral("Parameters"), this);
	auto* paramsLayout = new QVBoxLayout(paramsGroup);

	m_paramCountLabel = new QLabel(this);
	m_paramCountLabel->setStyleSheet("color: #a6adc8; font-size: 11px; margin-bottom: 2px;");
	paramsLayout->addWidget(m_paramCountLabel);

	m_paramTree = new QTreeWidget(this);
	m_paramTree->setHeaderLabels({
		QStringLiteral("Name"), QStringLiteral("Type"), QStringLiteral("Value") });
	m_paramTree->header()->setStretchLastSection(true);
	m_paramTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_paramTree->setAlternatingRowColors(true);
	m_paramTree->setRootIsDecorated(true);
	m_paramTree->setStyleSheet(
		"QTreeWidget { background:#1e1e2e; alternate-background-color:#252538; "
		"color:#cdd6f4; border:1px solid #313244; }"
		"QTreeWidget::item:selected { background:#45475a; }"
		"QHeaderView::section { background:#313244; color:#a6adc8; border:none; padding:4px; }");
	paramsLayout->addWidget(m_paramTree);

	splitter->addWidget(paramsGroup);

	splitter->setStretchFactor(0, 3);
	splitter->setStretchFactor(1, 2);

	mainLayout->addWidget(splitter);

	auto* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	m_closeBtn = new QPushButton(QStringLiteral("Close"), this);
	m_closeBtn->setFixedWidth(80);
	buttonLayout->addWidget(m_closeBtn);
	mainLayout->addLayout(buttonLayout);

	connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void UnitInfoDialog::loadUnitInfo(const ChemEngine::COBIA::UnitOperationInfo& info)
{
	m_unitInfo = info;

	QString name = QString::fromStdWString(info.name);
	QString progId = QString::fromStdWString(info.progId);
	QString vendor = QString::fromStdWString(info.vendor);
	QString desc = QString::fromStdWString(info.description);

	QString typeStr;
	switch (info.type)
	{
	case ChemEngine::COBIA::UnitOperationType::COBIA:
		typeStr = QStringLiteral("COBIA"); break;
	case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_1:
		typeStr = QStringLiteral("CAPE-OPEN 1.1"); break;
	case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_0:
		typeStr = QStringLiteral("CAPE-OPEN 1.0"); break;
	default:
		typeStr = QStringLiteral("Unknown"); break;
	}

	m_nameLabel->setText(name.isEmpty() ? QStringLiteral("(Unknown)") : name);
	m_typeLabel->setText(QStringLiteral("Type: %1").arg(typeStr));
	m_progIdLabel->setText(QStringLiteral("ProgID: %1").arg(progId));
	m_vendorLabel->setText(QStringLiteral("Vendor: %1").arg(vendor.isEmpty()
		? QStringLiteral("N/A") : vendor));
	m_descLabel->setText(desc.isEmpty() ? QStringLiteral("No description available.") : desc);

	m_statusLabel->clear();
	m_portTable->setRowCount(0);
	m_paramTree->clear();
	m_portCount = 0;
	m_paramCount = 0;

	auto* uoMgr = m_simMgr ? m_simMgr->unitOperationManager() : nullptr;
	if (!uoMgr || !info.isAvailable)
	{
		if (!info.isAvailable)
			m_statusLabel->setText(QStringLiteral("Unit is not available (cannot instantiate)."));
		populatePortList();
		populateParamTree();
		return;
	}

	try
	{
		::COBIA::CapeInterface unitIf = uoMgr->createUnit(info.progId);
		if (!unitIf)
		{
			m_statusLabel->setText(QStringLiteral("Failed to create unit instance."));
			populatePortList();
			populateParamTree();
			return;
		}

		ChemEngine::COBIA::CapeUnitWrapper wrapper;
		if (!wrapper.loadUnit(unitIf))
		{
			m_statusLabel->setText(QStringLiteral("Failed to load unit (possibly CAPE-OPEN 1.0)."));
			populatePortList();
			populateParamTree();
			return;
		}

		m_portCount = (int)wrapper.getPorts().size();

		try
		{
			CAPEOPEN_1_2::CapeUtilities capeUtils(unitIf);
			if (capeUtils)
			{
				auto params = capeUtils.getParameters();
				if (params)
					m_paramCount = (int)params.size();
			}
		}
		catch (...)
		{
		}
	}
	catch (...)
	{
		m_statusLabel->setText(QStringLiteral("Exception while loading unit."));
	}

	populatePortList();
	populateParamTree();
}

void UnitInfoDialog::loadBuiltInUnitInfo(const std::wstring& unitName)
{
	clearStreamContent();

	auto* fs = m_simMgr ? m_simMgr->flowsheet() : nullptr;
	if (!fs) return;

	auto* splitter = findChild<QSplitter*>();
	if (splitter) splitter->show();

	auto objPtr = fs->findObject(unitName);
	auto builtIn = std::dynamic_pointer_cast<ChemEngine::UnitOperationBase>(objPtr);
	if (!builtIn) return;

	const auto& ports = builtIn->getPorts();
	const auto& compounds = fs->getSelectedCompounds();

	m_nameLabel->setText(QString::fromStdWString(unitName));
	m_typeLabel->setText(QString::fromStdWString(builtIn->getObjectType()));
	m_progIdLabel->setText(QStringLiteral("ProgID: Built-In"));
	m_vendorLabel->setText(QStringLiteral("Vendor: ChemLab"));
	m_descLabel->setText(QStringLiteral("Built-in unit operation"));
	m_statusLabel->setText(builtIn->getValidationMessage().empty() ? QString() : QString::fromStdWString(builtIn->getValidationMessage()));
	m_statusLabel->setStyleSheet("color: #f38ba8; font-size: 11px;");

	m_portTable->setRowCount((int)ports.size());
	m_portCountLabel->setText(QStringLiteral("Total: %1 port(s)").arg((int)ports.size()));
	m_paramTree->clear();
	m_paramCountLabel->setText(QStringLiteral("Total: 0 parameter(s)"));

	int row = 0;
	for (const auto& port : ports)
	{
		QString dirStr = (port.direction == ChemEngine::PortDirection::Inlet)
			? QStringLiteral("INPUT") : QStringLiteral("OUTPUT");
		QColor dirColor = (port.direction == ChemEngine::PortDirection::Inlet)
			? QColor(0xa6, 0xe3, 0xa1) : QColor(0xf3, 0x8b, 0xa8);

		auto* nameItem = new QTableWidgetItem(QString::fromStdWString(port.name));
		auto* dirItem = new QTableWidgetItem(dirStr);
		dirItem->setForeground(dirColor);
		auto* typeItem = new QTableWidgetItem(QStringLiteral("Material"));
		auto* connItem = new QTableWidgetItem(
			port.connectedObjectName.empty()
				? QStringLiteral("(not connected)")
				: QString::fromStdWString(port.connectedObjectName));
		connItem->setForeground(port.connectedObjectName.empty()
			? QColor(0xf3, 0x8b, 0xa8) : QColor(0xa6, 0xe3, 0xa1));

		m_portTable->setItem(row, 0, nameItem);
		m_portTable->setItem(row, 1, dirItem);
		m_portTable->setItem(row, 2, typeItem);
		m_portTable->setItem(row, 3, connItem);
		++row;
	}

	QStringList propRows = {
		QStringLiteral("Temperature (K)"),
		QStringLiteral("Pressure (Pa)"),
		QStringLiteral("Molar Flow (mol/s)")
	};
	for (const auto& c : compounds)
		propRows.append(QString::fromStdWString(c.getConstantProperties().name) + QStringLiteral(" (mol frac)"));

	m_portCompareTable->setRowCount(propRows.size());
	m_portCompareTable->setColumnCount(0);
	m_portCompareTable->setVisible(true);

	int numPorts = (int)ports.size();
	m_portCompareTable->setColumnCount(numPorts + 1);

	QStringList headers;
	headers.append(QStringLiteral("Property"));
	for (const auto& port : ports)
		headers.append(QString::fromStdWString(port.name));
	m_portCompareTable->setHorizontalHeaderLabels(headers);

	for (int i = 0; i < propRows.size(); ++i)
	{
		auto* labelItem = new QTableWidgetItem(propRows[i]);
		labelItem->setForeground(QColor(0xa6, 0xad, 0xc8));
		m_portCompareTable->setItem(i, 0, labelItem);

		for (int j = 0; j < numPorts; ++j)
		{
			const auto& port = ports[j];
			auto* dataItem = new QTableWidgetItem(QStringLiteral("-"));
			dataItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

			if (!port.connectedObjectName.empty())
			{
				auto streamPtr = fs->findObject(port.connectedObjectName);
				auto* stream = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(streamPtr.get());
				if (stream)
				{
					if (i == 0)
						dataItem->setText(QString::number(stream->getTemperature(), 'f', 2));
					else if (i == 1)
						dataItem->setText(QString::number(stream->getPressure(), 'f', 1));
					else if (i == 2)
					{
						double mf = 0.0, maf = 0.0;
						stream->getOverallFlow(mf, maf);
						dataItem->setText(QString::number(mf, 'f', 4));
					}
					else
					{
						size_t compIdx = i - 3;
						auto fracs = stream->getMoleFractions();
						if (compIdx < fracs.size())
							dataItem->setText(QString::number(fracs[compIdx], 'f', 6));
					}

					QColor col = (port.direction == ChemEngine::PortDirection::Inlet)
						? QColor(0xa6, 0xe3, 0xa1) : QColor(0xfa, 0xb3, 0x87);
					dataItem->setForeground(col);
				}
			}

			m_portCompareTable->setItem(i, j + 1, dataItem);
		}
	}
}

void UnitInfoDialog::populatePortList()
{
	m_portTable->setRowCount(m_portCount);
	m_portCountLabel->setText(QStringLiteral("Total: %1 port(s)").arg(m_portCount));

	if (m_portCount == 0)
		return;

	auto* uoMgr = m_simMgr ? m_simMgr->unitOperationManager() : nullptr;
	if (!uoMgr)
		return;

	try
	{
		::COBIA::CapeInterface unitIf = uoMgr->createUnit(m_unitInfo.progId);
		if (!unitIf)
			return;

		ChemEngine::COBIA::CapeUnitWrapper wrapper;
		if (!wrapper.loadUnit(unitIf))
			return;

		const auto& ports = wrapper.getPorts();
		for (int row = 0; row < (int)ports.size(); ++row)
		{
			const auto& port = ports[row];

			QString dirStr;
			switch (port.direction)
			{
			case CAPEOPEN_1_2::CAPE_INLET:
				dirStr = QStringLiteral("INPUT");
				break;
			case CAPEOPEN_1_2::CAPE_OUTLET:
				dirStr = QStringLiteral("OUTPUT");
				break;
			case CAPEOPEN_1_2::CAPE_INLET_OUTLET:
				dirStr = QStringLiteral("IN/OUT");
				break;
			default:
				dirStr = QStringLiteral("?");
				break;
			}

			QString typeStr;
			switch (port.type)
			{
			case CAPEOPEN_1_2::CAPE_MATERIAL:
				typeStr = QStringLiteral("Material");
				break;
			case CAPEOPEN_1_2::CAPE_ENERGY:
				typeStr = QStringLiteral("Energy");
				break;
			case CAPEOPEN_1_2::CAPE_INFORMATION:
				typeStr = QStringLiteral("Information");
				break;
			default:
				typeStr = QStringLiteral("Other");
				break;
			}

			auto* nameItem = new QTableWidgetItem(QString::fromStdWString(port.name));
			auto* dirItem = new QTableWidgetItem(dirStr);
			auto* typeItem = new QTableWidgetItem(typeStr);
			auto* descItem = new QTableWidgetItem(QString::fromStdWString(port.description));

			QColor dirColor = (port.direction == CAPEOPEN_1_2::CAPE_INLET)
				? QColor(0xa6, 0xe3, 0xa1)
				: (port.direction == CAPEOPEN_1_2::CAPE_OUTLET)
					? QColor(0xf3, 0x8b, 0xa8)
					: QColor(0xf9, 0xe2, 0xaf);

			dirItem->setForeground(dirColor);

			m_portTable->setItem(row, 0, nameItem);
			m_portTable->setItem(row, 1, dirItem);
			m_portTable->setItem(row, 2, typeItem);
			m_portTable->setItem(row, 3, descItem);
		}
	}
	catch (...)
	{
		m_statusLabel->setText(QStringLiteral("Exception while reading ports."));
	}
}

void UnitInfoDialog::populateParamTree()
{
	m_paramCountLabel->setText(QStringLiteral("Total: %1 parameter(s)").arg(m_paramCount));

	if (m_paramCount == 0)
	{
		m_paramTree->addTopLevelItem(
			new QTreeWidgetItem({ QStringLiteral("(no parameters)") }));
		return;
	}

	auto* uoMgr = m_simMgr ? m_simMgr->unitOperationManager() : nullptr;
	if (!uoMgr)
		return;

	try
	{
		::COBIA::CapeInterface unitIf = uoMgr->createUnit(m_unitInfo.progId);
		if (!unitIf)
			return;

		ChemEngine::COBIA::CapeUnitWrapper wrapper;
		if (!wrapper.loadUnit(unitIf))
			return;

		CAPEOPEN_1_2::CapeUtilities capeUtils(unitIf);
		if (!capeUtils)
			return;

		auto params = capeUtils.getParameters();
		if (!params)
			return;

		int count = (int)params.size();
		for (int i = 0; i < count; ++i)
		{
			CAPEOPEN_1_2::CapeParameter param = params[(size_t)i];
			if (!param)
				continue;

			COBIA::CapeStringImpl n;
			CAPEOPEN_1_2::CapeIdentification pid(param);
			pid.getComponentName(n);
			QString pname = QString::fromStdWString(n.c_str());

			QString ptype;
			double val = 0.0;
			bool hasNum = false;

			try
			{
				switch (param.getType())
				{
				case CAPEOPEN_1_2::CAPE_PARAMETER_REAL:
					ptype = QStringLiteral("Real");
					val = CAPEOPEN_1_2::CapeRealParameter(param).getValue();
					hasNum = true;
					break;
				case CAPEOPEN_1_2::CAPE_PARAMETER_INTEGER:
					ptype = QStringLiteral("Integer");
					val = (double)CAPEOPEN_1_2::CapeIntegerParameter(param).getValue();
					hasNum = true;
					break;
				case CAPEOPEN_1_2::CAPE_PARAMETER_STRING:
					ptype = QStringLiteral("String");
					break;
				case CAPEOPEN_1_2::CAPE_PARAMETER_BOOLEAN:
					ptype = QStringLiteral("Boolean");
					break;
				default:
					ptype = QStringLiteral("Other");
					break;
				}
			}
			catch (...)
			{
			}

			QString valStr = hasNum ? QString::number(val, 'g', 6) : QStringLiteral("(non-numeric)");

			auto* item = new QTreeWidgetItem(m_paramTree, { pname, ptype, valStr });
			if (param.getMode() == CAPEOPEN_1_2::CAPE_INPUT)
			{
				item->setForeground(0, QColor(0xa6, 0xe3, 0xa1));
			}
			else if (param.getMode() == CAPEOPEN_1_2::CAPE_OUTPUT)
			{
				item->setForeground(0, QColor(0xf3, 0x8b, 0xa8));
			}
			else
			{
				item->setForeground(0, QColor(0xf9, 0xe2, 0xaf));
			}
		}
	}
	catch (...)
	{
		m_statusLabel->setText(
			QStringLiteral("%1 (parameters may not be supported by this unit).")
				.arg(m_statusLabel->text()));
	}
}

void UnitInfoDialog::setupStreamTreeView()
{
	m_streamTree = new QTreeWidget(this);
	m_streamTree->setHeaderLabels({
		QStringLiteral("Property"), QStringLiteral("Value"), QStringLiteral("Unit") });
	m_streamTree->header()->setStretchLastSection(true);
	m_streamTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_streamTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_streamTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	m_streamTree->setAlternatingRowColors(true);
	m_streamTree->setRootIsDecorated(true);
	m_streamTree->setAnimated(true);
	m_streamTree->setStyleSheet(
		"QTreeWidget { background:#1e1e2e; alternate-background-color:#252538; "
		"color:#cdd6f4; border:1px solid #313244; }"
		"QTreeWidget::item:selected { background:#45475a; }"
		"QTreeWidget::item:hover { background:#2a2a3c; }"
		"QHeaderView::section { background:#313244; color:#a6adc8; border:none; padding:4px; }"
		"QTreeWidget::item { padding:2px 0px; }");

	m_applyBtn = new QPushButton(QStringLiteral("Apply"), this);
	m_applyBtn->setFixedWidth(80);
	m_applyBtn->setStyleSheet(
		"QPushButton { background:#1e66f5; color:#cdd6f4; border:none; padding:6px 16px; }"
		"QPushButton:hover { background:#2e7af5; }");
	m_applyBtn->hide();

	}

void UnitInfoDialog::clearStreamContent()
{
	auto* mainLayout = qobject_cast<QVBoxLayout*>(layout());
	if (!mainLayout) return;

	auto* splitter = findChild<QSplitter*>();
	if (splitter) splitter->hide();

	for (int i = mainLayout->count() - 1; i >= 0; --i)
	{
		auto* item = mainLayout->itemAt(i);
		if (item && item->widget())
		{
			QGroupBox* gb = qobject_cast<QGroupBox*>(item->widget());
			if (gb && gb->objectName() != QStringLiteral("infoGroup"))
				gb->hide();
		}
	}

	if (m_streamTree)
	{
		m_streamTree->clear();
		m_streamTree->hide();
	}
	if (m_applyBtn)
		m_applyBtn->hide();

	m_portTable->setRowCount(0);
	m_portCountLabel->clear();
	m_paramTree->clear();
	m_paramCountLabel->clear();
	m_statusLabel->clear();

	m_basicCat = nullptr;
	m_thermoCat = nullptr;
	m_compositionCat = nullptr;
	m_phaseCat = nullptr;
	m_tempEdit = nullptr;
	m_pressEdit = nullptr;
	m_flowEdit = nullptr;
	m_tempItem = nullptr;
	m_pressItem = nullptr;
	m_flowItem = nullptr;
	m_stream = nullptr;
}

QTreeWidgetItem* UnitInfoDialog::addCategory(const QString& title)
{
	auto* item = new QTreeWidgetItem(m_streamTree, { title, QString(), QString() });
	item->setFlags(item->flags() | Qt::ItemIsEnabled);
	item->setExpanded(true);
	QFont f = item->font(0);
	f.setBold(true);
	f.setPointSize(f.pointSize() + 1);
	item->setFont(0, f);
	item->setForeground(0, QColor(0x89, 0xb4, 0xfa));
	item->setBackground(0, QColor(0x25, 0x25, 0x38));
	return item;
}

QTreeWidgetItem* UnitInfoDialog::addProperty(QTreeWidgetItem* parent,
	const QString& name, const QString& value, const QString& unit,
	const QColor& valColor)
{
	auto* item = new QTreeWidgetItem(parent);
	item->setText(0, name);
	item->setText(1, value);
	item->setText(2, unit);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	item->setForeground(0, QColor(0xa6, 0xad, 0xc8));
	item->setForeground(1, valColor);
	item->setForeground(2, QColor(0x6c, 0x70, 0x86));
	return item;
}

QTreeWidgetItem* UnitInfoDialog::addEditableProperty(QTreeWidgetItem* parent,
	const QString& name, double value, double min, double max, int decimals,
	const QString& suffix, const QColor& valColor)
{
	auto* item = new QTreeWidgetItem(parent);
	item->setText(0, name);
	item->setText(2, suffix);

	auto* spin = new QDoubleSpinBox(m_streamTree);
	spin->setRange(min, max);
	spin->setDecimals(decimals);
	spin->setValue(value);
	spin->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	spin->setStyleSheet(
		"QDoubleSpinBox { background:#313244; color:" + valColor.name() +
		"; border:1px solid #45475a; padding:1px 4px; }"
		"QDoubleSpinBox:focus { border:1px solid #89b4fa; }");
	spin->setFrame(false);

	m_streamTree->setItemWidget(item, 1, spin);
	item->setForeground(0, QColor(0xa6, 0xe3, 0xa1));

	return item;
}

void UnitInfoDialog::addCompositionRows(QTreeWidgetItem* parent, bool editable)
{
	if (!m_stream) return;
	auto* fs = m_stream->getFlowsheet();
	if (!fs) return;

	const auto& compounds = fs->getSelectedCompounds();
	if (compounds.empty()) return;

	auto moleFracs = m_stream->getMoleFractions();
	auto massFracs = m_stream->getMassFractions();
	size_t n = compounds.size();

	double defaultFrac = (n > 0) ? (1.0 / n) : 0.0;

	for (size_t i = 0; i < n; ++i)
	{
		double mf = (i < moleFracs.size()) ? moleFracs[i] : defaultFrac;
		double wf = (i < massFracs.size()) ? massFracs[i] : defaultFrac;

		auto* item = new QTreeWidgetItem(parent);
		item->setText(0, QString::fromStdWString(compounds[i].getConstantProperties().name));
		item->setText(1, QStringLiteral("%1  /  %2")
			.arg(mf, 0, 'f', 6)
			.arg(wf, 0, 'f', 6));
		item->setText(2, QStringLiteral("mole / mass"));
		item->setForeground(0, QColor(0xcd, 0xd6, 0xf4));

		if (editable)
		{
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			item->setForeground(1, QColor(0xa6, 0xe3, 0xa1));
		}
		else
		{
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			item->setForeground(1, QColor(0xf9, 0xe2, 0xaf));
		}
		item->setForeground(2, QColor(0x6c, 0x70, 0x86));

		item->setData(0, Qt::UserRole, QVariant::fromValue((qulonglong)i));
	}
}

void UnitInfoDialog::refreshCalculatedProperties()
{
	if (!m_stream || !m_thermoCat) return;

	if (m_basicCat)
	{
		if (m_tempItem)
		{
			m_tempItem->setText(1, QString::number(m_stream->getTemperature(), 'f', 4));
			if (m_tempEdit)
			{
				m_tempEdit->blockSignals(true);
				m_tempEdit->setValue(m_stream->getTemperature());
				m_tempEdit->blockSignals(false);
			}
		}
		if (m_pressItem)
		{
			m_pressItem->setText(1, QString::number(m_stream->getPressure(), 'f', 2));
			if (m_pressEdit)
			{
				m_pressEdit->blockSignals(true);
				m_pressEdit->setValue(m_stream->getPressure());
				m_pressEdit->blockSignals(false);
			}
		}
		if (m_flowItem)
		{
			double mf = 0.0, maf = 0.0;
			m_stream->getOverallFlow(mf, maf);
			m_flowItem->setText(1, QString::number(mf, 'f', 6));
			if (m_flowEdit)
			{
				m_flowEdit->blockSignals(true);
				m_flowEdit->setValue(mf);
				m_flowEdit->blockSignals(false);
			}
		}
	}

	auto* ppMgr = m_simMgr ? m_simMgr->propertyPackageManager() : nullptr;
	auto* matObj = ppMgr ? ppMgr->getMaterialObject() : nullptr;

	std::wstring primaryPhase = L"Overall";
	if (matObj)
	{
		const auto& phases = matObj->getPhaseLabels();
		if (!phases.empty())
			primaryPhase = phases[0];
	}

	struct ThermoProp {
		const wchar_t* prop;
		const char* label;
		const char* unit;
	};

	static const ThermoProp props[] = {
		{ L"enthalpy",           "Enthalpy",            "J/mol" },
		{ L"entropy",            "Entropy",             "J/(mol·K)" },
		{ L"density",            "Density",             "kg/m³" },
		{ L"heatCapacity",       "Cp (Heat Capacity)",  "J/(mol·K)" },
		{ L"viscosity",          "Viscosity",           "Pa·s" },
		{ L"thermalConductivity","Thermal Conductivity","W/(m·K)" },
		{ L"surfaceTension",     "Surface Tension",     "N/m" },
		{ L"heatOfVaporization", "Heat of Vaporization","J/mol" },
	};
	static const int numThermo = sizeof(props) / sizeof(props[0]);

	for (int idx = 0; idx < numThermo; ++idx)
	{
		const auto& p = props[idx];
		std::wstring propW(p.prop);
		double val = 0.0;

		if (matObj)
		{
			val = matObj->getCachedOverallProp(propW);
			if (val == 0.0 && propW != L"temperature" && propW != L"pressure")
			{
				val = matObj->getCachedSinglePhaseProp(propW, primaryPhase);
				if (val == 0.0 && primaryPhase != L"Overall")
					val = matObj->getCachedSinglePhaseProp(propW, L"Overall");
				if (val == 0.0)
				{
					for (const auto& ph : matObj->getPhaseLabels())
					{
						val = matObj->getCachedSinglePhaseProp(propW, ph);
						if (val != 0.0) break;
					}
				}
			}
			if (propW == L"density" && val > 0.0)
			{
				double mw = matObj->getMolecularWeight();
				if (mw > 0.0)
					val = val * mw * 1e-3;
			}
		}

		if (val == 0.0)
			val = m_stream->getOverallProp(propW, L"mole");

		QString vStr = QString::number(val, 'g', 6);
		if (propW == L"viscosity" || propW == L"surfaceTension")
			vStr = QString::number(val, 'e', 4);

		QTreeWidgetItem* item = nullptr;
		if (idx < m_thermoCat->childCount())
		{
			item = m_thermoCat->child(idx);
		}
		else
		{
			item = new QTreeWidgetItem(m_thermoCat);
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			item->setForeground(0, QColor(0xa6, 0xad, 0xc8));
			item->setForeground(2, QColor(0x6c, 0x70, 0x86));
		}

		item->setText(0, QString::fromUtf8(p.label));
		item->setText(1, vStr);
		item->setText(2, QString::fromUtf8(p.unit));
		item->setForeground(1, (val != 0.0 || propW == L"enthalpy" || propW == L"entropy")
			? QColor(0xcd, 0xd6, 0xf4) : QColor(0x58, 0x5b, 0x70));
	}

	while (m_thermoCat->childCount() > numThermo)
	{
		auto* child = m_thermoCat->child(numThermo);
		m_thermoCat->removeChild(child);
		delete child;
	}

	if (m_compositionCat)
	{
		auto* fs = m_stream->getFlowsheet();
		if (fs)
		{
			const auto& compounds = fs->getSelectedCompounds();
			auto moleFracs = m_stream->getMoleFractions();
			auto massFracs = m_stream->getMassFractions();
			int n = static_cast<int>(compounds.size());

			double defaultFrac = (n > 0) ? (1.0 / n) : 0.0;

			for (int i = 0; i < n; ++i)
			{
				double mf = (i < static_cast<int>(moleFracs.size())) ? moleFracs[i] : defaultFrac;
				double wf = (i < static_cast<int>(massFracs.size())) ? massFracs[i] : defaultFrac;

				QTreeWidgetItem* item = nullptr;
				if (i < m_compositionCat->childCount())
				{
					item = m_compositionCat->child(i);
				}
				else
				{
					item = new QTreeWidgetItem(m_compositionCat);
					item->setForeground(0, QColor(0xcd, 0xd6, 0xf4));
					item->setForeground(2, QColor(0x6c, 0x70, 0x86));
				}

				item->setText(0, QString::fromStdWString(compounds[i].getConstantProperties().name));
				item->setText(1, QStringLiteral("%1  /  %2").arg(mf, 0, 'f', 6).arg(wf, 0, 'f', 6));
				item->setText(2, QStringLiteral("mole / mass"));
				item->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<qulonglong>(i)));

				if (m_isFeed)
				{
					item->setFlags(item->flags() | Qt::ItemIsEditable);
					item->setForeground(1, QColor(0xa6, 0xe3, 0xa1));
				}
				else
				{
					item->setFlags(item->flags() & ~Qt::ItemIsEditable);
					item->setForeground(1, QColor(0xf9, 0xe2, 0xaf));
				}
			}

			while (m_compositionCat->childCount() > n)
			{
				auto* child = m_compositionCat->child(n);
				m_compositionCat->removeChild(child);
				delete child;
			}
		}
	}

	if (m_phaseCat)
	{
		int nPhases = static_cast<int>(m_stream->getPhaseCount());
		for (int i = 0; i < nPhases; ++i)
		{
			std::wstring label = m_stream->getPhaseLabel(i);
			double frac = m_stream->getPhaseFraction(label, L"mole");

			QTreeWidgetItem* item = nullptr;
			if (i < m_phaseCat->childCount())
			{
				item = m_phaseCat->child(i);
			}
			else
			{
				item = new QTreeWidgetItem(m_phaseCat);
				item->setFlags(item->flags() & ~Qt::ItemIsEditable);
				item->setForeground(0, QColor(0xa6, 0xad, 0xc8));
				item->setForeground(1, QColor(0x89, 0xb4, 0xfa));
				item->setForeground(2, QColor(0x6c, 0x70, 0x86));
			}

			item->setText(0, QString::fromStdWString(label));
			item->setText(1, QString::number(frac, 'f', 6));
			item->setText(2, QStringLiteral("mole"));
		}

		while (m_phaseCat->childCount() > nPhases)
		{
			auto* child = m_phaseCat->child(nPhases);
			m_phaseCat->removeChild(child);
			delete child;
		}

		m_phaseCat->setHidden(nPhases == 0);
	}

	if (m_isFeed)
	{
		double currentMolarFlow = 0.0, currentMassFlow = 0.0;
		m_stream->getOverallFlow(currentMolarFlow, currentMassFlow);
		if (m_flowEdit)
		{
			m_flowEdit->blockSignals(true);
			m_flowEdit->setValue(currentMolarFlow);
			m_flowEdit->blockSignals(false);
		}
	}
}

void UnitInfoDialog::applyFeedProperties()
{
	if (!m_stream) return;

	auto* ppMgr = m_simMgr ? m_simMgr->propertyPackageManager() : nullptr;
	if (!ppMgr) return;

	auto* activePkg = ppMgr->getActivePackage();
	if (!activePkg) return;

	const auto& pkCompounds = ppMgr->getCompoundsFromPackage();
	auto* fs = m_stream->getFlowsheet();
	const auto& fsCompounds = fs ? fs->getSelectedCompounds() : ChemEngine::CompoundList();
	const auto& compounds = !pkCompounds.empty() ? pkCompounds : fsCompounds;

	if (compounds.empty()) return;

	if (!m_stream->hasCompounds())
		m_stream->addCompounds(compounds);

	ppMgr->createMaterialObject();
	ppMgr->setMaterialCompounds(compounds);
	auto* matObj = ppMgr->getMaterialObject();

	if (!matObj) return;

	double T = m_tempEdit ? m_tempEdit->value() : m_stream->getTemperature();
	double P = m_pressEdit ? m_pressEdit->value() : m_stream->getPressure();

	m_stream->setTemperature(T);
	m_stream->setPressure(P);
	matObj->setOverallTemperature(T);
	matObj->setOverallPressure(P);

	if (m_flowEdit)
{
    m_stream->setTotalMolarFlow(m_flowEdit->value());
    matObj->setOverallFlow(m_flowEdit->value());
}

	if (m_compositionCat)
	{
		std::vector<double> moleFracs;
		int n = m_compositionCat->childCount();
		for (int i = 0; i < n; ++i)
		{
			auto* item = m_compositionCat->child(i);
			QStringList parts = item->text(1).split(QStringLiteral("  /  "));
			double mf = parts.value(0).trimmed().toDouble();
			moleFracs.push_back(mf);
		}
		if (!moleFracs.empty())
		{
			double sum = 0.0;
			for (double x : moleFracs) sum += x;
			if (sum > 1e-12)
			{
				for (double& x : moleFracs) x /= sum;
			}
			else
			{
				for (double& x : moleFracs) x = 1.0 / n;
			}
			m_stream->setMoleFractions(moleFracs);
			matObj->setOverallComposition(moleFracs);
		}
	}

	ppMgr->setMaterial(m_stream);
	if (ppMgr->calculateEquilibrium())
	{
		std::vector<std::wstring> calcProps = {
			L"enthalpy", L"entropy", L"density",
			L"heatCapacity", L"viscosity", L"thermalConductivity",
			L"surfaceTension", L"heatOfVaporization"
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

		m_stream->setCapeMaterialObject(matObj);
	}

	m_stream->flashTP();
	refreshCalculatedProperties();
}

void UnitInfoDialog::loadStreamResults(ChemEngine::COBIA::CapeMaterialStream& stream)
{
	setWindowTitle(QStringLiteral("Material Stream - %1")
		.arg(QString::fromStdWString(stream.getObjectName())));

	m_nameLabel->setText(QString::fromStdWString(stream.getObjectName()));
	m_typeLabel->setText(QStringLiteral("Type: Material Stream (Results)"));
	m_progIdLabel->clear();
	m_vendorLabel->clear();
	m_descLabel->setText(QStringLiteral("Material stream properties and composition."));

	clearStreamContent();
	setupStreamTreeView();

	m_stream = &stream;
	m_isFeed = false;

	auto* mainLayout = qobject_cast<QVBoxLayout*>(layout());
	if (mainLayout)
	{
		mainLayout->insertWidget(mainLayout->count() - 1, m_streamTree);
	}

	populateStreamProperties(false);

	if (m_applyBtn) m_applyBtn->hide();
	m_streamTree->show();
}

void UnitInfoDialog::loadFeedStreamEditor(ChemEngine::COBIA::CapeMaterialStream& stream)
{
	setWindowTitle(QStringLiteral("Feed Stream - %1")
		.arg(QString::fromStdWString(stream.getObjectName())));

	m_nameLabel->setText(QString::fromStdWString(stream.getObjectName()));
	m_typeLabel->setText(QStringLiteral("Type: Feed Stream (Editable)"));
	m_progIdLabel->clear();
	m_vendorLabel->clear();
	m_descLabel->setText(QStringLiteral("Set temperature, pressure, flow rate and composition."));

	clearStreamContent();
	setupStreamTreeView();

	m_stream = &stream;
	m_isFeed = true;

	auto* mainLayout = qobject_cast<QVBoxLayout*>(layout());
	if (mainLayout)
	{
		mainLayout->insertWidget(mainLayout->count() - 1, m_streamTree);
	}

	populateStreamProperties(true);

	if (mainLayout)
	{
		auto* buttonLayout = qobject_cast<QHBoxLayout*>(
			mainLayout->itemAt(mainLayout->count() - 1) ? mainLayout->itemAt(mainLayout->count() - 1)->layout() : nullptr);
		if (buttonLayout)
		{
			m_applyBtn->show();
			buttonLayout->insertWidget(0, m_applyBtn);
			disconnect(m_applyBtn, nullptr, this, nullptr);
			connect(m_applyBtn, &QPushButton::clicked, this, &UnitInfoDialog::applyFeedProperties);
		}
	}

	m_streamTree->show();

	connect(m_streamTree, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int column) {
		if (column != 1) return;
		if (!item || !m_stream) return;
		if (item->parent() != m_compositionCat) return;

		QStringList parts = item->text(1).split(QStringLiteral("  /  "));
		double mf = parts.value(0).trimmed().toDouble();
		double wf = parts.value(1).trimmed().toDouble();

		auto* fs = m_stream->getFlowsheet();
		double MW = 1.0;
		auto idxVar = item->data(0, Qt::UserRole);
		if (idxVar.isValid() && fs)
		{
			size_t idx = idxVar.value<qulonglong>();
			const auto& compounds = fs->getSelectedCompounds();
			if (idx < compounds.size())
				MW = compounds[idx].getConstantProperties().molecularWeight;
		}

		if (mf > 0.0 && wf <= 0.0)
		{
			double sum = 0.0;
			for (int i = 0; i < m_compositionCat->childCount(); ++i)
			{
				auto* sib = m_compositionCat->child(i);
				if (sib == item) continue;
				QStringList sp = sib->text(1).split(QStringLiteral("  /  "));
				sum += sp.value(0).trimmed().toDouble();
			}
			sum += mf;
			if (sum > 0.0)
			{
				double mwSum = 0.0;
				for (int i = 0; i < m_compositionCat->childCount(); ++i)
				{
					auto* sib = m_compositionCat->child(i);
					QStringList sp = sib->text(1).split(QStringLiteral("  /  "));
					double smf = sp.value(0).trimmed().toDouble() / sum;
					auto sibIdxVar = sib->data(0, Qt::UserRole);
					double sibMW = 1.0;
					if (sibIdxVar.isValid() && fs)
					{
						size_t sibIdx = sibIdxVar.value<qulonglong>();
						const auto& comps = fs->getSelectedCompounds();
						if (sibIdx < comps.size())
							sibMW = comps[sibIdx].getConstantProperties().molecularWeight;
					}
					mwSum += smf * sibMW;
				}
				wf = (mf / sum) * MW / mwSum;
			}
		}

		m_streamTree->blockSignals(true);
		item->setText(1, QStringLiteral("%1  /  %2")
			.arg(mf, 0, 'f', 6).arg(wf, 0, 'f', 6));
		m_streamTree->blockSignals(false);

		applyFeedProperties();
	});
}

void UnitInfoDialog::populateStreamProperties(bool isFeed)
{
	if (!m_stream || !m_streamTree) return;

	double T = m_stream->getTemperature();
	double P = m_stream->getPressure();
	double molarFlow = 0.0, massFlow = 0.0;
	m_stream->getOverallFlow(molarFlow, massFlow);

	m_basicCat = addCategory(QStringLiteral("Basic Properties"));

	if (isFeed)
	{
		m_tempItem = addEditableProperty(m_basicCat, QStringLiteral("Temperature"),
			T, 0.0, 10000.0, 4, QStringLiteral("K"), QColor(0xa6, 0xe3, 0xa1));
		m_tempEdit = qobject_cast<QDoubleSpinBox*>(m_streamTree->itemWidget(m_tempItem, 1));
		if (m_tempEdit)
		{
			connect(m_tempEdit, &QDoubleSpinBox::editingFinished,
				this, &UnitInfoDialog::applyFeedProperties);
		}

		m_pressItem = addEditableProperty(m_basicCat, QStringLiteral("Pressure"),
			P, 0.0, 1e15, 4, QStringLiteral("Pa"), QColor(0xa6, 0xe3, 0xa1));
		m_pressEdit = qobject_cast<QDoubleSpinBox*>(m_streamTree->itemWidget(m_pressItem, 1));
		if (m_pressEdit)
		{
			connect(m_pressEdit, &QDoubleSpinBox::editingFinished,
				this, &UnitInfoDialog::applyFeedProperties);
		}

		m_flowItem = addEditableProperty(m_basicCat, QStringLiteral("Molar Flow"),
			molarFlow, 0.0, 1e10, 6, QStringLiteral("mol/s"), QColor(0xa6, 0xe3, 0xa1));
		m_flowEdit = qobject_cast<QDoubleSpinBox*>(m_streamTree->itemWidget(m_flowItem, 1));
		if (m_flowEdit)
		{
			connect(m_flowEdit, &QDoubleSpinBox::editingFinished,
				this, &UnitInfoDialog::applyFeedProperties);
		}
	}
	else
	{
		addProperty(m_basicCat, QStringLiteral("Temperature"),
			QStringLiteral("%1 K  (%2 °C, %3 °F)")
				.arg(T, 0, 'f', 2)
				.arg(T - 273.15, 0, 'f', 2)
				.arg(T * 9.0 / 5.0 - 459.67, 0, 'f', 2),
			QString(), QColor(0xcd, 0xd6, 0xf4));
		addProperty(m_basicCat, QStringLiteral("Pressure"),
			QStringLiteral("%1 Pa  (%2 bar, %3 atm, %4 psi)")
				.arg(P, 0, 'f', 1)
				.arg(P / 1e5, 0, 'f', 4)
				.arg(P / 101325.0, 0, 'f', 4)
				.arg(P / 6894.757, 0, 'f', 4),
			QString(), QColor(0xcd, 0xd6, 0xf4));
		addProperty(m_basicCat, QStringLiteral("Molar Flow"),
			QStringLiteral("%1 mol/s").arg(molarFlow, 0, 'f', 6),
			QString(), QColor(0xcd, 0xd6, 0xf4));
		addProperty(m_basicCat, QStringLiteral("Mass Flow"),
			QStringLiteral("%1 kg/s").arg(massFlow, 0, 'f', 6),
			QString(), QColor(0xcd, 0xd6, 0xf4));
	}

	m_thermoCat = addCategory(QStringLiteral("Thermodynamic Properties"));

	m_compositionCat = addCategory(QStringLiteral("Composition"));
	addCompositionRows(m_compositionCat, isFeed);

	m_phaseCat = addCategory(QStringLiteral("Phase Distribution"));

	refreshCalculatedProperties();
}