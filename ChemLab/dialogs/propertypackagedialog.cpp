#include "propertypackagedialog.h"
#include "propertyresearchdialog.h"
#include "../simulationmanager.h"

#include "ChemEngine/COBIA/PropertyPackageManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QApplication>

PropertyPackageDialog::PropertyPackageDialog(SimulationManager* simMgr, QWidget* parent)
	: QDialog(parent)
	, m_simMgr(simMgr)
	, m_table(nullptr)
	, m_refreshBtn(nullptr)
	, m_loadBtn(nullptr)
	, m_closeBtn(nullptr)
	, m_statusLabel(nullptr)
{
	setupUi();
}

void PropertyPackageDialog::setupUi()
{
	setWindowTitle(QStringLiteral("Property Package Library"));
	setMinimumSize(780, 480);
	resize(820, 520);

	setStyleSheet(QStringLiteral(R"(
		PropertyPackageDialog {
			background: #1e1e2e;
		}
		QLabel {
			color: #cdd6f4;
			font-size: 11px;
		}
	)"));

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(16, 16, 16, 16);
	mainLayout->setSpacing(12);

	auto* titleLabel = new QLabel(QStringLiteral("COBIA / CAPE-OPEN Property Packages"), this);
	titleLabel->setStyleSheet(QStringLiteral(
		"font-size: 16px; font-weight: 600; color: #cdd6f4; padding-bottom: 4px;"));
	mainLayout->addWidget(titleLabel);

	auto* filterLayout = new QHBoxLayout();
	filterLayout->setSpacing(8);

	auto* filterLabel = new QLabel(QStringLiteral("Filter:"), this);
	filterLabel->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 11px;"));
	filterLayout->addWidget(filterLabel);

	m_typeFilter = new QComboBox(this);
	m_typeFilter->addItem(QStringLiteral("All Types"), 0);
	m_typeFilter->addItem(QStringLiteral("COBIA"), 1);
	m_typeFilter->addItem(QStringLiteral("CAPE-OPEN"), 2);
	m_typeFilter->setCurrentIndex(0);
	m_typeFilter->setStyleSheet(QStringLiteral(R"(
		QComboBox {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			border-radius: 4px;
			padding: 4px 12px;
			font-size: 11px;
			min-width: 140px;
		}
		QComboBox:hover {
			border-color: #45475a;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 20px;
			border-left: 1px solid #2d2d3f;
		}
		QComboBox QAbstractItemView {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			selection-background-color: #313244;
		}
	)"));
	connect(m_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &PropertyPackageDialog::onFilterChanged);
	filterLayout->addWidget(m_typeFilter);

	auto* cobiaBadge = new QLabel(this);
	cobiaBadge->setText(QStringLiteral("COBIA"));
	cobiaBadge->setStyleSheet(QStringLiteral(
		"background: rgba(74,158,255,0.2); color: #4a9eff; "
		"border: 1px solid rgba(74,158,255,0.35); border-radius: 3px; "
		"padding: 2px 8px; font-size: 10px; font-weight: 600;"));
	filterLayout->addWidget(cobiaBadge);

	auto* capeBadge = new QLabel(this);
	capeBadge->setText(QStringLiteral("CAPE-OPEN"));
	capeBadge->setStyleSheet(QStringLiteral(
		"background: rgba(166,173,200,0.15); color: #a6adc8; "
		"border: 1px solid rgba(166,173,200,0.25); border-radius: 3px; "
		"padding: 2px 8px; font-size: 10px; font-weight: 600;"));
	filterLayout->addWidget(capeBadge);

	filterLayout->addStretch();
	mainLayout->addLayout(filterLayout);

	m_table = new QTableWidget(this);
	m_table->setColumnCount(6);
	m_table->setHorizontalHeaderLabels({
		QStringLiteral("Name"),
		QStringLiteral("ProgID"),
		QStringLiteral("Type"),
		QStringLiteral("Vendor"),
		QStringLiteral("Version"),
		QStringLiteral("Status")
	});
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->setAlternatingRowColors(true);
	m_table->verticalHeader()->setVisible(false);
	m_table->horizontalHeader()->setStretchLastSection(true);
	m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
	m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

	m_table->setStyleSheet(QStringLiteral(R"(
		QTableWidget {
			background: #181825;
			color: #cdd6f4;
			border: 1px solid #2d2d3f;
			border-radius: 6px;
			gridline-color: #2d2d3f;
			font-size: 11px;
			outline: none;
			selection-background-color: #313244;
			selection-color: #cdd6f4;
		}
		QTableWidget::item {
			padding: 5px 10px;
		}
		QTableWidget::item:hover {
			background: #252536;
		}
		QHeaderView::section {
			background: #11111b;
			color: #a6adc8;
			border: none;
			border-bottom: 2px solid #313244;
			padding: 7px 10px;
			font-size: 11px;
			font-weight: 600;
		}
	)"));

	mainLayout->addWidget(m_table, 1);

	m_statusLabel = new QLabel(this);
	m_statusLabel->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 11px;"));
	mainLayout->addWidget(m_statusLabel);

	auto* btnLayout = new QHBoxLayout();
	btnLayout->setSpacing(10);
	btnLayout->addStretch();

	m_refreshBtn = new QPushButton(QStringLiteral("Refresh"), this);
	m_refreshBtn->setStyleSheet(QStringLiteral(R"(
		QPushButton {
			background: #313244;
			color: #cdd6f4;
			border: 1px solid #45475a;
			border-radius: 4px;
			padding: 6px 18px;
			font-size: 12px;
		}
		QPushButton:hover {
			background: #45475a;
			border-color: #585b70;
		}
		QPushButton:pressed {
			background: #585b70;
		}
	)"));
	connect(m_refreshBtn, &QPushButton::clicked, this, &PropertyPackageDialog::onRefresh);
	btnLayout->addWidget(m_refreshBtn);

	m_loadBtn = new QPushButton(QStringLiteral("Load && Study"), this);
	m_loadBtn->setStyleSheet(QStringLiteral(R"(
		QPushButton {
			background: #4a9eff;
			color: #1e1e2e;
			border: none;
			border-radius: 4px;
			padding: 6px 18px;
			font-size: 12px;
			font-weight: 600;
		}
		QPushButton:hover {
			background: #6bb5ff;
		}
		QPushButton:pressed {
			background: #3a8eef;
		}
		QPushButton:disabled {
			background: #252536;
			color: #585b70;
		}
	)"));
	connect(m_loadBtn, &QPushButton::clicked, this, &PropertyPackageDialog::onLoad);
	btnLayout->addWidget(m_loadBtn);

	m_studyBtn = new QPushButton(QStringLiteral("Study"), this);
	m_studyBtn->setStyleSheet(QStringLiteral(R"(
		QPushButton {
			background: #313244;
			color: #cdd6f4;
			border: 1px solid #45475a;
			border-radius: 4px;
			padding: 6px 18px;
			font-size: 12px;
		}
		QPushButton:hover {
			background: #45475a;
			border-color: #585b70;
		}
		QPushButton:pressed {
			background: #585b70;
		}
		QPushButton:disabled {
			background: #252536;
			color: #585b70;
		}
	)"));
	m_studyBtn->setEnabled(false);
	connect(m_studyBtn, &QPushButton::clicked, this, &PropertyPackageDialog::onStudy);
	btnLayout->addWidget(m_studyBtn);

	m_closeBtn = new QPushButton(QStringLiteral("Close"), this);
	m_closeBtn->setStyleSheet(QStringLiteral(R"(
		QPushButton {
			background: #313244;
			color: #cdd6f4;
			border: 1px solid #45475a;
			border-radius: 4px;
			padding: 6px 18px;
			font-size: 12px;
		}
		QPushButton:hover {
			background: #45475a;
			border-color: #585b70;
		}
		QPushButton:pressed {
			background: #585b70;
		}
	)"));
	connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
	btnLayout->addWidget(m_closeBtn);

	mainLayout->addLayout(btnLayout);
}

void PropertyPackageDialog::populateTable()
{
	if (!m_simMgr || !m_simMgr->propertyPackageManager())
		return;

	const auto& pkgs = m_simMgr->propertyPackageManager()->getPackages();

	m_table->setRowCount(0);
	m_table->setRowCount(static_cast<int>(pkgs.size()));

	int visibleCount = 0;
	int cobCount = 0;
	int capeCount = 0;

	for (int i = 0; i < static_cast<int>(pkgs.size()); ++i)
	{
		const auto& pkg = pkgs[i];

		bool isCOBIA = (pkg.type == ChemEngine::COBIA::PropertyPackageType::COBIA);
		bool isCAPE = (pkg.type == ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_2
			|| pkg.type == ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_1);

		if (m_filterType == 1 && !isCOBIA)
		{
			m_table->setRowHidden(i, true);
			continue;
		}
		if (m_filterType == 2 && !isCAPE)
		{
			m_table->setRowHidden(i, true);
			continue;
		}
		m_table->setRowHidden(i, false);
		visibleCount++;

		if (isCOBIA) cobCount++;
		if (isCAPE) capeCount++;

		auto* nameItem = new QTableWidgetItem(QString::fromStdWString(pkg.name));
		nameItem->setData(Qt::UserRole, QString::fromStdWString(pkg.progId));
		m_table->setItem(i, 0, nameItem);

		m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdWString(pkg.progId)));

		QString typeStr;
		QColor typeColor;
		QColor rowTint;
		if (isCOBIA)
		{
			typeStr = QStringLiteral("COBIA");
			typeColor = QColor(0x4a, 0x9e, 0xff);
			rowTint = QColor(26, 32, 55);
		}
		else if (pkg.type == ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_2)
		{
			typeStr = QStringLiteral("CAPE-OPEN 1.2");
			typeColor = QColor(0xa6, 0xad, 0xc8);
			rowTint = QColor(28, 28, 35);
		}
		else if (pkg.type == ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_1)
		{
			typeStr = QStringLiteral("CAPE-OPEN 1.1");
			typeColor = QColor(0x93, 0x99, 0xb2);
			rowTint = QColor(28, 28, 35);
		}
		else
		{
			typeStr = QStringLiteral("Unknown");
			typeColor = QColor(0xf3, 0x8b, 0xa8);
			rowTint = QColor(28, 28, 35);
		}
		auto* typeItem = new QTableWidgetItem(typeStr);
		typeItem->setForeground(typeColor);
		QFont typeFont = typeItem->font();
		typeFont.setBold(isCOBIA);
		typeItem->setFont(typeFont);
		m_table->setItem(i, 2, typeItem);

		m_table->setItem(i, 3, new QTableWidgetItem(
			pkg.vendor.empty() ? QStringLiteral("-") : QString::fromStdWString(pkg.vendor)));
		m_table->setItem(i, 4, new QTableWidgetItem(
			pkg.version.empty() ? QStringLiteral("-") : QString::fromStdWString(pkg.version)));

		auto* statusItem = new QTableWidgetItem(pkg.isAvailable
			? QStringLiteral("Available") : QStringLiteral("Unavailable"));
		statusItem->setForeground(pkg.isAvailable ? QColor(0x00, 0xc8, 0x53) : QColor(0xf3, 0x8b, 0xa8));
		m_table->setItem(i, 5, statusItem);

		for (int col = 0; col < m_table->columnCount(); ++col)
		{
			QTableWidgetItem* it = m_table->item(i, col);
			if (it)
			{
				it->setBackground(rowTint);
				if (pkg.isActive)
				{
					QFont f = it->font();
					f.setBold(true);
					it->setFont(f);
				}
			}
		}

		if (isCOBIA && !pkg.isActive)
		{
			for (int col = 0; col < m_table->columnCount(); ++col)
			{
				QTableWidgetItem* it = m_table->item(i, col);
				if (it)
				{
					QBrush bg = it->background();
					QColor c = bg.color();
					it->setBackground(c.lighter(108));
				}
			}
		}
	}

	if (m_filterType == 0)
		m_statusLabel->setText(QStringLiteral("Found %1 package(s) — %2 COBIA, %3 CAPE-OPEN")
			.arg(visibleCount).arg(cobCount).arg(capeCount));
	else if (m_filterType == 1)
		m_statusLabel->setText(QStringLiteral("COBIA packages: %1 found").arg(visibleCount));
	else
		m_statusLabel->setText(QStringLiteral("CAPE-OPEN packages: %1 found").arg(visibleCount));
}

void PropertyPackageDialog::onRefresh()
{
	if (!m_simMgr)
		return;

	m_simMgr->enumeratePropertyPackages();
	populateTable();
}

void PropertyPackageDialog::onFilterChanged(int index)
{
	m_filterType = m_typeFilter ? m_typeFilter->itemData(index).toInt() : 0;
	populateTable();
}

void PropertyPackageDialog::onLoad()
{
	int row = m_table->currentRow();
	if (row < 0)
	{
		QMessageBox::information(this,
			QStringLiteral("Select Package"),
			QStringLiteral("Please select a property package from the list first."));
		return;
	}

	QString progId = m_table->item(row, 0)->data(Qt::UserRole).toString();
	QString name = m_table->item(row, 0)->text();
	QString typeStr = m_table->item(row, 2)->text();
	bool isCOBIA = (typeStr == QStringLiteral("COBIA"));

	if (progId.isEmpty())
		return;

	if (isCOBIA)
	{
		m_statusLabel->setStyleSheet(QStringLiteral("color: #4a9eff; font-size: 11px;"));
		m_statusLabel->setText(QStringLiteral("Loading COBIA package: %1 ...").arg(name));
	}
	else
	{
		m_statusLabel->setStyleSheet(QStringLiteral("color: #a6adc8; font-size: 11px;"));
		m_statusLabel->setText(QStringLiteral("Loading CAPE-OPEN package: %1 ...").arg(name));
	}

	QApplication::processEvents();

	bool ok = m_simMgr->loadPropertyPackage(progId.toStdWString());
	if (ok)
	{
		if (isCOBIA)
		{
			m_statusLabel->setStyleSheet(QStringLiteral("color: #4a9eff; font-size: 11px; font-weight: 600;"));
			m_statusLabel->setText(QStringLiteral("[COBIA] Loaded: %1").arg(name));
		}
		else
		{
			m_statusLabel->setStyleSheet(QStringLiteral("color: #00c853; font-size: 11px;"));
			m_statusLabel->setText(QStringLiteral("[CAPE-OPEN] Loaded: %1").arg(name));
		}
		m_studyBtn->setEnabled(true);
		populateTable();
		emit packageLoaded(name);

		openStudy();
	}
	else
	{
		m_statusLabel->setStyleSheet(QStringLiteral("color: #f38ba8; font-size: 11px;"));
		m_statusLabel->setText(QStringLiteral("Failed to load: %1 (%2)")
			.arg(name).arg(typeStr));
		m_studyBtn->setEnabled(false);
	}
}

void PropertyPackageDialog::onStudy()
{
	openStudy();
}

void PropertyPackageDialog::openStudy()
{
	auto* ppMgr = m_simMgr ? m_simMgr->propertyPackageManager() : nullptr;
	if (!ppMgr) return;

	auto* activePkg = ppMgr->getActivePackage();
	if (!activePkg)
	{
		QMessageBox::information(this,
			QStringLiteral("No Package"),
			QStringLiteral("Please load a Property Package first."));
		return;
	}

	auto* dlg = new PropertyResearchDialog(m_simMgr, this);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setActivePackage(activePkg->progId);
	dlg->show();
}