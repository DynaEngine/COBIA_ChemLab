#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QDockWidget>
#include <QTreeWidget>
#include <QStatusBar>
#include <memory>

namespace QWK {
	class WidgetWindowAgent;
	class WindowButton;
}

class RibbonBar;
class SimulationManager;
class UnitOperationPanel;
class PropertyPackageDialog;
class UnitInfoDialog;
class FlowsheetScene;
class FlowsheetGraphicsView;
class LogPanel;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

	RibbonBar* ribbonBar() const;
	QTabWidget* tabWidget() const;
	SimulationManager* simulationManager() const;

public slots:
	int addFlowsheetTab(const QString& title, QWidget* widget);

private slots:
	void onFileMenuClicked();
	void onTabChanged(int index);
	void onSolveRequested();
	void onQuickDemo();
	void onEngineLog(const QString& msg);
	void onPropertyPackageDialog();
	void onNewFlowsheet();
	void onUnitDoubleClicked(const QString& name, const QString& progId);
	void onSolverConfig();
	void onShowResultsOverview();
	void onTabContextMenu(const QPoint& pos);

protected:
	bool event(QEvent* event) override;

private:
	void setupUi();
	void installWindowAgent();
	void setupRibbonContent();
	void setupDockPanels();

	QWK::WidgetWindowAgent* m_windowAgent = nullptr;
	RibbonBar* m_ribbon = nullptr;
	QTabWidget* m_tabWidget = nullptr;

	SimulationManager* m_simManager = nullptr;

	QDockWidget* m_dockNav = nullptr;
	UnitOperationPanel* m_unitOpPanel = nullptr;

	QDockWidget* m_dockLog = nullptr;
	LogPanel* m_logPanel = nullptr;

	PropertyPackageDialog* m_ppDialog = nullptr;
	UnitInfoDialog* m_unitInfoDialog = nullptr;

	FlowsheetScene* m_canvasScene = nullptr;
	FlowsheetGraphicsView* m_canvasView = nullptr;

	int m_simTabIndex = -1;
};