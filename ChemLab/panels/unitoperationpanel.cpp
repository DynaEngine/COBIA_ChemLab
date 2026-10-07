#include "unitoperationpanel.h"
#include "../simulationmanager.h"

#include "ChemEngine/COBIA/UnitOperationManager.h"
#include "ChemEngine/Units/UnitOperationBase.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

static QIcon makeUnitIcon(const QColor& color)
{
	QPixmap pix(16, 16);
	pix.fill(Qt::transparent);
	QPainter p(&pix);
	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(Qt::NoPen);
	p.setBrush(color);
	p.drawRoundedRect(QRectF(1, 1, 14, 14), 3, 3);
	p.end();
	return QIcon(pix);
}

UnitOperationPanel::UnitOperationPanel(SimulationManager* simMgr, QWidget* parent)
	: QWidget(parent)
	, m_simMgr(simMgr)
	, m_tree(nullptr)
	, m_refreshBtn(nullptr)
	, m_viewToggle(nullptr)
	, m_statusLabel(nullptr)
{
	setupUi();
}

void UnitOperationPanel::setupUi()
{
	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(6, 6, 6, 6);
	mainLayout->setSpacing(6);

	auto* header = new QHBoxLayout();
	auto* label = new QLabel(QStringLiteral("Unit Operations"), this);
	label->setStyleSheet("font-weight: bold; font-size: 12px; color: #4a9eff;");
	header->addWidget(label);
	header->addStretch();

	m_viewToggle = new QToolButton(this);
	m_viewToggle->setIcon(QIcon(QStringLiteral(":/icons/stream.svg")));
	m_viewToggle->setToolTip(QStringLiteral("Toggle flat / tree view"));
	m_viewToggle->setFixedSize(24, 24);
	m_viewToggle->setCheckable(true);
	m_viewToggle->setChecked(false);
	header->addWidget(m_viewToggle);

	m_refreshBtn = new QPushButton(this);
	m_refreshBtn->setIcon(QIcon(QStringLiteral(":/icons/refresh.svg")));
	m_refreshBtn->setToolTip(QStringLiteral("Refresh unit operation list from system"));
	m_refreshBtn->setFixedSize(24, 24);
	m_refreshBtn->setFlat(true);
	header->addWidget(m_refreshBtn);
	mainLayout->addLayout(header);

	m_tree = new UnitTreeWidget(this);
	m_tree->setHeaderHidden(true);
	m_tree->setMinimumHeight(80);
	m_tree->setIndentation(14);
	m_tree->setIconSize(QSize(16, 16));
	m_tree->setDragEnabled(true);
	m_tree->setDragDropMode(QAbstractItemView::DragOnly);
	mainLayout->addWidget(m_tree);

	m_statusLabel = new QLabel(QStringLiteral("No unit selected"), this);
	m_statusLabel->setWordWrap(true);
	mainLayout->addWidget(m_statusLabel);

	connect(m_refreshBtn, &QPushButton::clicked, [this]() {
		m_simMgr->enumerateUnitOperations();
		refreshUnitList();
	});

	connect(m_viewToggle, &QToolButton::toggled, [this](bool checked) {
		m_flatView = checked;
		refreshUnitList();
	});

	connect(m_tree, &QTreeWidget::itemClicked,
			this, &UnitOperationPanel::onItemClicked);

	connect(m_tree, &QTreeWidget::itemDoubleClicked,
			this, &UnitOperationPanel::onItemDoubleClicked);
}

void UnitOperationPanel::onItemClicked(QTreeWidgetItem* item, int /*col*/)
{
	if (!item) return;

	QString progId = item->data(0, Qt::UserRole).toString();
	QString name = item->data(0, Qt::UserRole + 1).toString();
	if (progId.isEmpty() && name.isEmpty()) return;

	QString typeStr = item->data(0, Qt::UserRole + 2).toString();
	QString vendor = item->data(0, Qt::UserRole + 3).toString();

	m_statusLabel->setText(
		QStringLiteral("<span style='color:#4a9eff;font-weight:bold;'>%1</span><br/>"
			"<span style='color:#a6adc8;'>%2 | %3 | %4</span>")
		.arg(name, progId.isEmpty() ? typeStr : progId, typeStr, vendor));
	emit unitSelected(name, progId);
}

void UnitOperationPanel::onItemDoubleClicked(QTreeWidgetItem* item, int /*col*/)
{
	if (!item) return;

	QString progId = item->data(0, Qt::UserRole).toString();
	QString name = item->data(0, Qt::UserRole + 1).toString();
	if (progId.isEmpty() && name.isEmpty()) return;

	emit unitDoubleClicked(name, progId);
}

void UnitOperationPanel::refreshUnitList()
{
	if (m_flatView)
		populateFlatList(true);
	else
		populateTree();
}

void UnitOperationPanel::addBuiltInSection(QTreeWidgetItem* parent)
{
	struct BuiltInInfo {
		QString name;
		QString typeName;
		QString inPorts;
		QString outPorts;
		QString icon;
	};

	QList<BuiltInInfo> builtins = {
		{ QStringLiteral("Mixer"),      QStringLiteral("mixer"),  QStringLiteral("Inlet1|Inlet2"), QStringLiteral("Outlet"), QStringLiteral(":/icons/mixer.svg") },
		{ QStringLiteral("Heater"),     QStringLiteral("heater"), QStringLiteral("Inlet"), QStringLiteral("Outlet"), QStringLiteral(":/icons/heater.svg") },
		{ QStringLiteral("Flash Drum"), QStringLiteral("flash"),  QStringLiteral("Inlet"), QStringLiteral("VaporOutlet|LiquidOutlet"), QStringLiteral(":/icons/flash.svg") },
		{ QStringLiteral("Valve"),      QStringLiteral("valve"),  QStringLiteral("Inlet"), QStringLiteral("Outlet"), QStringLiteral(":/icons/valve.svg") },
		{ QStringLiteral("Splitter"),   QStringLiteral("splitter"), QStringLiteral("Inlet"), QStringLiteral("Outlet1|Outlet2"), QStringLiteral(":/icons/splitter.svg") },
		{ QStringLiteral("Cooler"),     QStringLiteral("cooler"), QStringLiteral("Inlet"), QStringLiteral("Outlet"), QStringLiteral(":/icons/cooler.svg") },
		{ QStringLiteral("Compressor"), QStringLiteral("compressor"), QStringLiteral("Inlet"), QStringLiteral("Outlet"), QStringLiteral(":/icons/compressor.svg") },
		{ QStringLiteral("Pump"),       QStringLiteral("pump"),   QStringLiteral("Inlet"), QStringLiteral("Outlet"), QStringLiteral(":/icons/pump.svg") },
	};

	auto* streamsNode = new QTreeWidgetItem(parent, { QStringLiteral("Streams") });
	streamsNode->setExpanded(true);
	streamsNode->setFlags(streamsNode->flags() & ~Qt::ItemIsSelectable);
	QFont boldFont;
	boldFont.setBold(true);
	streamsNode->setFont(0, boldFont);

	auto* matStreamItem = new QTreeWidgetItem(streamsNode, { QStringLiteral("Material Stream") });
	matStreamItem->setIcon(0, makeUnitIcon(QColor(0xfa, 0xb3, 0x87)));
	matStreamItem->setData(0, Qt::UserRole, QStringLiteral("builtin"));
	matStreamItem->setData(0, Qt::UserRole + 1, QStringLiteral("Material Stream"));
	matStreamItem->setData(0, Qt::UserRole + 2, QStringLiteral("Stream"));
	matStreamItem->setData(0, Qt::UserRole + 3, QStringLiteral("ChemLab"));
	matStreamItem->setData(0, Qt::UserRole + 5, QStringLiteral("materialstream"));
	matStreamItem->setData(0, Qt::UserRole + 6, QStringLiteral("inlet"));
	matStreamItem->setData(0, Qt::UserRole + 7, QStringLiteral("outlet"));
	matStreamItem->setFlags(matStreamItem->flags() | Qt::ItemIsDragEnabled);

	auto* energyStreamItem = new QTreeWidgetItem(streamsNode, { QStringLiteral("Energy Stream") });
	energyStreamItem->setIcon(0, makeUnitIcon(QColor(0xff, 0x98, 0x00)));
	energyStreamItem->setData(0, Qt::UserRole, QStringLiteral("builtin"));
	energyStreamItem->setData(0, Qt::UserRole + 1, QStringLiteral("Energy Stream"));
	energyStreamItem->setData(0, Qt::UserRole + 2, QStringLiteral("Stream"));
	energyStreamItem->setData(0, Qt::UserRole + 3, QStringLiteral("ChemLab"));
	energyStreamItem->setData(0, Qt::UserRole + 5, QStringLiteral("energystream"));
	energyStreamItem->setData(0, Qt::UserRole + 6, QStringLiteral("inlet"));
	energyStreamItem->setData(0, Qt::UserRole + 7, QStringLiteral("outlet"));
	energyStreamItem->setFlags(energyStreamItem->flags() | Qt::ItemIsDragEnabled);

	auto* signalStreamItem = new QTreeWidgetItem(streamsNode, { QStringLiteral("Signal Stream") });
	signalStreamItem->setIcon(0, makeUnitIcon(QColor(0xab, 0x47, 0xbc)));
	signalStreamItem->setData(0, Qt::UserRole, QStringLiteral("builtin"));
	signalStreamItem->setData(0, Qt::UserRole + 1, QStringLiteral("Signal Stream"));
	signalStreamItem->setData(0, Qt::UserRole + 2, QStringLiteral("Stream"));
	signalStreamItem->setData(0, Qt::UserRole + 3, QStringLiteral("ChemLab"));
	signalStreamItem->setData(0, Qt::UserRole + 5, QStringLiteral("signalstream"));
	signalStreamItem->setData(0, Qt::UserRole + 6, QStringLiteral("inlet"));
	signalStreamItem->setData(0, Qt::UserRole + 7, QStringLiteral("outlet"));
	signalStreamItem->setFlags(signalStreamItem->flags() | Qt::ItemIsDragEnabled);

	auto* node = new QTreeWidgetItem(parent, { QStringLiteral("Built-In Units") });
	node->setExpanded(true);
	node->setFlags(node->flags() & ~Qt::ItemIsSelectable);
	node->setFont(0, boldFont);

	for (const auto& bi : builtins)
	{
		auto* item = new QTreeWidgetItem(node, { bi.name });
		item->setIcon(0, QIcon(bi.icon));
		item->setData(0, Qt::UserRole, QStringLiteral("builtin"));
		item->setData(0, Qt::UserRole + 1, bi.name);
		item->setData(0, Qt::UserRole + 2, QStringLiteral("Built-In"));
		item->setData(0, Qt::UserRole + 3, QStringLiteral("ChemLab"));
		item->setData(0, Qt::UserRole + 5, bi.typeName);
		item->setData(0, Qt::UserRole + 6, bi.inPorts);
		item->setData(0, Qt::UserRole + 7, bi.outPorts);
		item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
	}
}

void UnitOperationPanel::addCOBIASection(QTreeWidgetItem* parent)
{
	auto* uoMgr = m_simMgr->unitOperationManager();
	if (!uoMgr) return;

	const auto& units = uoMgr->getUnits();
	if (units.empty()) return;

	auto* node = new QTreeWidgetItem(parent, { QStringLiteral("CAPE-OPEN / COBIA") });
	node->setExpanded(true);
	node->setFlags(node->flags() & ~Qt::ItemIsSelectable);
	QFont boldFont;
	boldFont.setBold(true);
	node->setFont(0, boldFont);

	QMap<QString, QTreeWidgetItem*> typeNodes;
	QMap<QString, QTreeWidgetItem*> vendorNodes;

	static const QIcon cobiaIcon = makeUnitIcon(QColor(0x4a, 0x9e, 0xff));
	static const QIcon cape11Icon = makeUnitIcon(QColor(0x00, 0xc8, 0x53));
	static const QIcon cape10Icon = makeUnitIcon(QColor(0xa6, 0xad, 0xc8));
	static const QIcon unknownIcon = makeUnitIcon(QColor(0x6c, 0x70, 0x86));

	for (const auto& unit : units)
	{
		QString typeStr;
		QIcon unitIcon;
		switch (unit.type)
		{
		case ChemEngine::COBIA::UnitOperationType::COBIA:
			typeStr = QStringLiteral("COBIA");
			unitIcon = cobiaIcon;
			break;
		case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_1:
			typeStr = QStringLiteral("CAPE-OPEN 1.1");
			unitIcon = cape11Icon;
			break;
		case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_0:
			typeStr = QStringLiteral("CAPE-OPEN 1.0");
			unitIcon = cape10Icon;
			break;
		default:
			typeStr = QStringLiteral("Unknown");
			unitIcon = unknownIcon;
			break;
		}

		QString vendor = QString::fromStdWString(unit.vendor);
		if (vendor.isEmpty()) vendor = QStringLiteral("Unknown / Standalone");

		if (!typeNodes.contains(typeStr))
		{
			auto* tn = new QTreeWidgetItem(node, { typeStr });
			tn->setExpanded(true);
			tn->setFlags(tn->flags() & ~Qt::ItemIsSelectable);
			typeNodes[typeStr] = tn;
		}

		QString vk = typeStr + QStringLiteral("::") + vendor;
		if (!vendorNodes.contains(vk))
		{
			auto* vn = new QTreeWidgetItem(typeNodes[typeStr], { vendor });
			vn->setExpanded(true);
			vn->setFlags(vn->flags() & ~Qt::ItemIsSelectable);
			vn->setForeground(0, QColor(0xa6, 0xad, 0xc8));
			vendorNodes[vk] = vn;
		}

		QString label = QString::fromStdWString(unit.name);

		auto* item = new QTreeWidgetItem(vendorNodes[vk], { label });
		item->setIcon(0, unitIcon);
		item->setData(0, Qt::UserRole, QString::fromStdWString(unit.progId));
		item->setData(0, Qt::UserRole + 1, QString::fromStdWString(unit.name));
		item->setData(0, Qt::UserRole + 2, typeStr);
		item->setData(0, Qt::UserRole + 3, vendor);
		item->setData(0, Qt::UserRole + 5, QString::fromStdWString(unit.name));
		item->setData(0, Qt::UserRole + 6, QString());
		item->setData(0, Qt::UserRole + 7, QString());
		item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
		item->setToolTip(0, QStringLiteral("Name: %1\nType: %2\nProgID: %3\nVersion: %4\nVendor: %5")
			.arg(QString::fromStdWString(unit.name))
			.arg(typeStr)
			.arg(QString::fromStdWString(unit.progId))
			.arg(QString::fromStdWString(unit.version))
			.arg(QString::fromStdWString(unit.vendor)));
	}
}

void UnitOperationPanel::populateTree()
{
	m_tree->blockSignals(true);
	m_tree->clear();

	addBuiltInSection(m_tree->invisibleRootItem());
	addCOBIASection(m_tree->invisibleRootItem());

	m_tree->blockSignals(false);
}

void UnitOperationPanel::populateFlatList(bool show)
{
	Q_UNUSED(show);
	populateTree();
}