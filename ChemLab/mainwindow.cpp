#include "mainwindow.h"
#include "ribbonbar.h"
#include "simulationmanager.h"
#include "panels/unitoperationpanel.h"
#include "panels/logpanel.h"
#include "canvas/FlowsheetScene.h"
#include "canvas/FlowsheetGraphicsView.h"
#include "dialogs/propertypackagedialog.h"
#include "dialogs/unitinfodialog.h"
#include "dialogs/configdialog.h"
#include "theme/theme.h"

#include "ChemEngine/COBIA/UnitOperationManager.h"

#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"

#include <QWKWidgets/widgetwindowagent.h>
#include <QWKWidgets/windowbutton.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QStyle>
#include <QTextEdit>
#include <QLabel>
#include <QMargins>
#include <QMdiSubWindow>
#include <QSplitter>
#include <QToolBar>
#include <QDrag>
#include <QTabBar>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QTimer>

static void mwLog(const QString& msg)
{
	QFile f(QDir::tempPath() + "/ChemLab_startup.log");
	if (f.open(QIODevice::Append | QIODevice::Text)) {
		QTextStream ts(&f);
		ts << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "  [MW] " << msg << "\n";
	}
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_windowAgent(nullptr)
	, m_ribbon(nullptr)
	, m_tabWidget(nullptr)
	, m_simManager(nullptr)
	, m_dockNav(nullptr)
	, m_unitOpPanel(nullptr)
	, m_simTabIndex(-1)
{
	ChemLabTheme::applyDarkTheme();
	mwLog("Theme applied, calling setupUi()");
	setupUi();
	mwLog("setupUi() completed");
}

MainWindow::~MainWindow() = default;

RibbonBar* MainWindow::ribbonBar() const
{
	return m_ribbon;
}

QTabWidget* MainWindow::tabWidget() const
{
	return m_tabWidget;
}

SimulationManager* MainWindow::simulationManager() const
{
	return m_simManager;
}

int MainWindow::addFlowsheetTab(const QString& title, QWidget* widget)
{
	if (!m_tabWidget || !widget)
		return -1;

	int index = m_tabWidget->addTab(widget, title);
	m_tabWidget->setCurrentIndex(index);
	return index;
}

bool MainWindow::event(QEvent* e)
{
	return QMainWindow::event(e);
}

void MainWindow::setupUi()
{
	setWindowTitle(QStringLiteral("ChemLab"));
	setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
	setMinimumSize(900, 550);

	setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);

	m_ribbon = new RibbonBar(this);
	setMenuWidget(m_ribbon);

	connect(m_ribbon, &RibbonBar::currentTabChanged, this, &MainWindow::onTabChanged);

	mwLog("installWindowAgent()");
	installWindowAgent();

	mwLog("Creating SimulationManager");
	m_simManager = new SimulationManager(this);
	m_simManager->initialize();

	connect(m_simManager, &SimulationManager::logMessage, this, &MainWindow::onEngineLog);

	mwLog("setupDockPanels()");
	setupDockPanels();

	mwLog("Creating TabWidget");
	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setTabsClosable(true);
	m_tabWidget->setMovable(true);
	m_tabWidget->setDocumentMode(true);
	m_tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tabWidget->tabBar(), &QTabBar::customContextMenuRequested,
			this, &MainWindow::onTabContextMenu);
	connect(m_tabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
		QWidget* w = m_tabWidget->widget(index);
		m_tabWidget->removeTab(index);
		delete w;
	});
	setCentralWidget(m_tabWidget);

	QStatusBar* sb = statusBar();
	sb->setFixedHeight(24);
	sb->showMessage(QStringLiteral(" Ready"));

	mwLog("onNewFlowsheet()");
	onNewFlowsheet();

	mwLog("setupRibbonContent()");
	setupRibbonContent();
	mwLog("setupUi() end");
}

void MainWindow::setupDockPanels()
{
	m_dockNav = new QDockWidget(QStringLiteral("Unit Operations"), this);
	m_dockNav->setObjectName(QStringLiteral("navDock"));
	m_dockNav->setFeatures(QDockWidget::DockWidgetMovable |
		QDockWidget::DockWidgetFloatable |
		QDockWidget::DockWidgetClosable);
	m_dockNav->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	m_unitOpPanel = new UnitOperationPanel(m_simManager, m_dockNav);
	m_dockNav->setWidget(m_unitOpPanel);
	addDockWidget(Qt::LeftDockWidgetArea, m_dockNav);

	connect(m_unitOpPanel, &UnitOperationPanel::unitDoubleClicked,
			this, &MainWindow::onUnitDoubleClicked);

	m_dockLog = new QDockWidget(QStringLiteral("Log"), this);
	m_dockLog->setObjectName(QStringLiteral("logDock"));
	m_dockLog->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
		QDockWidget::DockWidgetClosable |
		QDockWidget::DockWidgetMovable);
	m_dockLog->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

	m_logPanel = new LogPanel(m_dockLog);
	m_dockLog->setWidget(m_logPanel);

	addDockWidget(Qt::BottomDockWidgetArea, m_dockLog);

	connect(m_simManager, &SimulationManager::logMessage,
			m_logPanel, &LogPanel::appendMessage);
}

void MainWindow::installWindowAgent()
{
	m_windowAgent = new QWK::WidgetWindowAgent(this);
	m_windowAgent->setup(this);

	m_windowAgent->setWindowAttribute(QStringLiteral("dark-mode"), true);
	m_windowAgent->setWindowAttribute(QStringLiteral("extra-margins"),
		QVariant::fromValue(QMargins(0, 1, 0, 0)));

	auto iconButton = new QWK::WindowButton();
	iconButton->setObjectName(QStringLiteral("icon-button"));
	iconButton->setCheckable(false);
	iconButton->setIconNormal(QIcon(QStringLiteral(":/icons/chemlab_logo.svg")));

	auto minButton = new QWK::WindowButton();
	minButton->setObjectName(QStringLiteral("min-button"));
	minButton->setProperty("system-button", true);
	minButton->setIconNormal(QApplication::style()->standardIcon(QStyle::SP_TitleBarMinButton));

	auto maxButton = new QWK::WindowButton();
	maxButton->setObjectName(QStringLiteral("max-button"));
	maxButton->setProperty("system-button", true);
	maxButton->setCheckable(true);
	maxButton->setIconNormal(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	maxButton->setIconChecked(QApplication::style()->standardIcon(QStyle::SP_TitleBarNormalButton));

	auto closeButton = new QWK::WindowButton();
	closeButton->setObjectName(QStringLiteral("close-button"));
	closeButton->setProperty("system-button", true);
	closeButton->setIconNormal(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	m_ribbon->setIconButton(iconButton);
	m_ribbon->setMinButton(minButton);
	m_ribbon->setMaxButton(maxButton);
	m_ribbon->setCloseButton(closeButton);

	m_windowAgent->setTitleBar(m_ribbon->titleBarWidget());
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
    m_windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);

    m_windowAgent->setHitTestVisible(m_ribbon->leftCorner(), true);
    m_windowAgent->setHitTestVisible(m_ribbon->tabBarWidget(), true);

	connect(minButton, &QAbstractButton::clicked, this, &QWidget::showMinimized);
	connect(maxButton, &QAbstractButton::clicked, this, [this, maxButton]() {
		if (isMaximized()) {
			showNormal();
		}
		else {
			showMaximized();
		}
		maxButton->setChecked(isMaximized());
		});
	connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);
}

void MainWindow::setupRibbonContent()
{
	int homeTab = m_ribbon->addTab(QStringLiteral("Home"));
	m_ribbon->addGroup(homeTab, QStringLiteral("Flowsheet"));
	QAction* actNew = m_ribbon->addLargeButton(homeTab, QStringLiteral("Flowsheet"),
		QStringLiteral("New"),
		QIcon(QStringLiteral(":/icons/new.svg")));

	connect(actNew, &QAction::triggered, this, &MainWindow::onNewFlowsheet);
	
	m_ribbon->addLargeButton(homeTab, QStringLiteral("Flowsheet"),
		QStringLiteral("Open"),
		QIcon(QStringLiteral(":/icons/open.svg")));
	m_ribbon->addLargeButton(homeTab, QStringLiteral("Flowsheet"),
		QStringLiteral("Save"),
		QIcon(QStringLiteral(":/icons/save.svg")));

	m_ribbon->addGroup(homeTab, QStringLiteral("Material"));
	QAction* actPropertyLib = m_ribbon->addLargeButton(homeTab, QStringLiteral("Material"),
		QStringLiteral("Property\nLibrary"),
		QIcon(QStringLiteral(":/icons/ppackage.svg")));
	connect(actPropertyLib, &QAction::triggered, this, &MainWindow::onPropertyPackageDialog);

	m_simTabIndex = m_ribbon->addTab(QStringLiteral("Simulation"));

	QAction* actNewFs = m_ribbon->addLargeButton(m_simTabIndex, QStringLiteral("Flowsheet"),
		QStringLiteral("New\nFlowsheet"),
		QIcon(QStringLiteral(":/icons/new.svg")));
	connect(actNewFs, &QAction::triggered, this, &MainWindow::onNewFlowsheet);

	QAction* actClear = m_ribbon->addSmallButton(m_simTabIndex, QStringLiteral("Flowsheet"),
		QStringLiteral("Clear"),
		QIcon(QStringLiteral(":/icons/delete.svg")));
	connect(actClear, &QAction::triggered, [this]() {
		m_simManager->clearFlowsheet();
		if (m_canvasScene) m_canvasScene->clearFlowsheet();
	});

	m_ribbon->addGroup(m_simTabIndex, QStringLiteral("Solver"));
	QAction* actSolve = m_ribbon->addLargeButton(m_simTabIndex, QStringLiteral("Solver"),
		QStringLiteral("Solve"),
		QIcon(QStringLiteral(":/icons/solve.svg")));
	connect(actSolve, &QAction::triggered, this, &MainWindow::onSolveRequested);

	QAction* actSolverCfg = m_ribbon->addLargeButton(m_simTabIndex, QStringLiteral("Solver"),
		QStringLiteral("Config"),
		QIcon(QStringLiteral(":/icons/config.svg")));
	connect(actSolverCfg, &QAction::triggered, this, &MainWindow::onSolverConfig);

	m_ribbon->addGroup(m_simTabIndex, QStringLiteral("Results"));
	QAction* actResults = m_ribbon->addLargeButton(m_simTabIndex, QStringLiteral("Results"),
		QStringLiteral("Results"),
		QIcon(QStringLiteral(":/icons/status_done.svg")));
	connect(actResults, &QAction::triggered, this, &MainWindow::onShowResultsOverview);
}

void MainWindow::onNewFlowsheet()
{
	m_simManager->clearFlowsheet();

	QWidget* flowsheetWidget = new QWidget();
	auto* layout = new QVBoxLayout(flowsheetWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_canvasScene = new FlowsheetScene(m_simManager, flowsheetWidget);
	auto* canvasView = new FlowsheetGraphicsView(m_canvasScene, m_simManager, flowsheetWidget);
	m_canvasView = canvasView;
	layout->addWidget(canvasView);

	addFlowsheetTab(QStringLiteral("Flowsheet 1"), flowsheetWidget);

	connect(m_canvasScene, &FlowsheetScene::statusMessage, [this](const QString& msg) {
		statusBar()->showMessage(msg, 5000);
	});
}

void MainWindow::onFileMenuClicked()
{
	QMenu menu(this);

	QAction* actNew = menu.addAction(QIcon(QStringLiteral(":/icons/new.svg")), QStringLiteral("New"));
	QAction* actOpen = menu.addAction(QIcon(QStringLiteral(":/icons/open.svg")), QStringLiteral("Open..."));
	QAction* actSave = menu.addAction(QIcon(QStringLiteral(":/icons/save.svg")), QStringLiteral("Save"));
	menu.addSeparator();
	QAction* actExit = menu.addAction(QStringLiteral("Exit"));

	QAction* chosen = menu.exec(m_ribbon->titleBarWidget()->mapToGlobal(QPoint(0, m_ribbon->titleBarWidget()->height())));

	if (chosen == actExit)
	{
		close();
	}
	else if (chosen == actNew)
	{
		m_simManager->clearFlowsheet();
		onNewFlowsheet();
	}
}

void MainWindow::onTabChanged(int index)
{
	Q_UNUSED(index);
}

void MainWindow::onTabContextMenu(const QPoint& pos)
{
	QTabBar* bar = m_tabWidget->tabBar();
	int tabIndex = bar->tabAt(pos);
	if (tabIndex < 0) return;

	QMenu menu(this);
	QAction* actClose = menu.addAction(QStringLiteral("Close"));
	menu.addSeparator();
	QAction* actCloseOthers = menu.addAction(QStringLiteral("Close Others"));
	QAction* actCloseAll = menu.addAction(QStringLiteral("Close All"));

	if (m_tabWidget->count() <= 1) {
		actCloseOthers->setEnabled(false);
		actCloseAll->setEnabled(false);
	}

	QAction* chosen = menu.exec(bar->mapToGlobal(pos));

	if (chosen == actClose) {
		QWidget* w = m_tabWidget->widget(tabIndex);
		m_tabWidget->removeTab(tabIndex);
		delete w;
	} else if (chosen == actCloseOthers) {
		for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
			if (i != tabIndex) {
				QWidget* w = m_tabWidget->widget(i);
				m_tabWidget->removeTab(i);
				delete w;
			}
		}
	} else if (chosen == actCloseAll) {
		while (m_tabWidget->count() > 0) {
			QWidget* w = m_tabWidget->widget(0);
			m_tabWidget->removeTab(0);
			delete w;
		}
	}
}

void MainWindow::onSolveRequested()
{
	statusBar()->showMessage(QStringLiteral(" Solving flowsheet..."));

	bool result = m_simManager->solve();

	if (result)
	{
		statusBar()->showMessage(QStringLiteral(" Solve completed."));
	}
	else
	{
		statusBar()->showMessage(QStringLiteral(" Solve failed."));
	}
}

void MainWindow::onQuickDemo()
{
	m_simManager->quickDemo();
	onNewFlowsheet();
}

void MainWindow::onEngineLog(const QString& msg)
{
	Q_UNUSED(msg);
}

void MainWindow::onPropertyPackageDialog()
{
	if (!m_ppDialog)
	{
		m_ppDialog = new PropertyPackageDialog(m_simManager, this);
		connect(m_ppDialog, &PropertyPackageDialog::packageLoaded, [this](const QString& name) {
			statusBar()->showMessage(QStringLiteral("Property Package loaded: %1").arg(name), 3000);
		});
	}
	m_ppDialog->onRefresh();
	m_ppDialog->show();
	m_ppDialog->raise();
}

void MainWindow::onSolverConfig()
{
	ConfigDialog dlg(m_simManager, this);
	dlg.exec();
}

void MainWindow::onShowResultsOverview()
{
	auto* fs = m_simManager->flowsheet();
	if (!fs) return;

	auto* dlg = new QDialog(this);
	dlg->setWindowTitle(QStringLiteral("Results Overview"));
	dlg->resize(700, 450);
	dlg->setAttribute(Qt::WA_DeleteOnClose);

	auto* layout = new QVBoxLayout(dlg);
	layout->setContentsMargins(12, 12, 12, 12);

	auto* table = new QTableWidget(dlg);
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({
		QStringLiteral("Name"), QStringLiteral("Type"), QStringLiteral("Status"), QStringLiteral("Details") });
	table->horizontalHeader()->setStretchLastSection(true);
	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setAlternatingRowColors(true);
	table->verticalHeader()->setVisible(false);

	const auto& objects = fs->getObjects();
	table->setRowCount(static_cast<int>(objects.size()));

	for (size_t i = 0; i < objects.size(); ++i)
	{
		const auto& obj = objects[i];
		int row = static_cast<int>(i);

		table->setItem(row, 0, new QTableWidgetItem(
			QString::fromStdWString(obj.name)));

		QString typeStr = (obj.type == ChemEngine::FlowsheetObjectType::MaterialStream)
			? QStringLiteral("Stream")
			: QStringLiteral("Unit");
		table->setItem(row, 1, new QTableWidgetItem(typeStr));

		auto status = obj.object ? obj.object->getStatus()
			: ChemEngine::SimulationObjectStatus::NotCalculated;
		QString statusStr;
		switch (status)
		{
		case ChemEngine::SimulationObjectStatus::Calculated:
			statusStr = QStringLiteral("Calculated"); break;
		case ChemEngine::SimulationObjectStatus::Error:
			statusStr = QStringLiteral("Error"); break;
		case ChemEngine::SimulationObjectStatus::Calculating:
			statusStr = QStringLiteral("Calculating..."); break;
		default:
			statusStr = QStringLiteral("Not calculated"); break;
		}
		auto* statusItem = new QTableWidgetItem(statusStr);
		if (status == ChemEngine::SimulationObjectStatus::Calculated)
			statusItem->setForeground(QColor(0xa6, 0xe3, 0xa1));
		else if (status == ChemEngine::SimulationObjectStatus::Error)
			statusItem->setForeground(QColor(0xf3, 0x8b, 0xa8));
		table->setItem(row, 2, statusItem);

		QString details;
		if (obj.type == ChemEngine::FlowsheetObjectType::MaterialStream)
		{
			auto stream = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeMaterialStream>(obj.object);
			if (stream && status == ChemEngine::SimulationObjectStatus::Calculated)
			{
				details = QStringLiteral("T=%1 K, P=%2 Pa")
					.arg(stream->getTemperature(), 0, 'f', 2)
					.arg(stream->getPressure(), 0, 'f', 1);
			}
		}
		else
		{
			auto unit = std::dynamic_pointer_cast<ChemEngine::COBIA::CapeUnitWrapper>(obj.object);
			if (unit && status == ChemEngine::SimulationObjectStatus::Calculated)
			{
				details = QStringLiteral("%1 ports").arg(unit->getPorts().size());
			}
		}
		table->setItem(row, 3, new QTableWidgetItem(details));
	}

	layout->addWidget(table);

	auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Close, dlg);
	connect(btnBox, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
	layout->addWidget(btnBox);

	dlg->show();
}

void MainWindow::onUnitDoubleClicked(const QString& name, const QString& progId)
{
	Q_UNUSED(name);
	Q_UNUSED(progId);
}