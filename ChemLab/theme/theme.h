#pragma once

#include <QString>
#include <QApplication>

namespace ChemLabTheme {

	inline void applyDarkTheme()
	{
		const char* darkTheme = R"(
* {
	font-family: "Microsoft YaHei", sans-serif;
}

QMainWindow {
	background: #1e1e2e;
}

QMainWindow::separator {
	width: 1px;
	background: #2d2d3f;
}

QDockWidget {
	color: #cdd6f4;
	background: #1e1e2e;
	border: none;
	titlebar-close-icon: none;
	titlebar-normal-icon: none;
}

QDockWidget::title {
	background: #181825;
	padding: 6px 10px;
	border-bottom: 1px solid #2d2d3f;
	font-size: 12px;
	font-weight: 600;
	color: #cdd6f4;
	text-align: left;
}

QMenuBar {
	background: #181825;
	color: #cdd6f4;
	border-bottom: 1px solid #2d2d3f;
	padding: 2px;
}

QMenuBar::item {
	padding: 4px 12px;
	background: transparent;
	border-radius: 4px;
}

QMenuBar::item:selected {
	background: #313244;
}

QMenu {
	background: #1e1e2e;
	color: #cdd6f4;
	border: 1px solid #313244;
	padding: 4px;
	border-radius: 6px;
}

QMenu::item {
	padding: 6px 28px 6px 14px;
	border-radius: 4px;
}

QMenu::item:selected {
	background: #313244;
}

QMenu::separator {
	height: 1px;
	background: #2d2d3f;
	margin: 4px 8px;
}

QTreeWidget {
	background: #181825;
	color: #cdd6f4;
	border: 1px solid #2d2d3f;
	border-radius: 4px;
	font-size: 11px;
	outline: none;
}

QTreeWidget::item {
	padding: 4px 6px;
	height: 26px;
	border-radius: 3px;
}

QTreeWidget::item:hover {
	background: #252536;
}

QTreeWidget::item:selected {
	background: #313244;
	color: #cdd6f4;
}

QTreeWidget::branch {
	background: transparent;
}

QTableWidget, QTableView {
	background: #181825;
	color: #cdd6f4;
	border: 1px solid #2d2d3f;
	border-radius: 4px;
	gridline-color: #2d2d3f;
	font-size: 11px;
	outline: none;
	selection-background-color: #313244;
	selection-color: #cdd6f4;
}

QTableWidget::item, QTableView::item {
	padding: 3px 8px;
}

QTableWidget::item:hover {
	background: #252536;
}

QHeaderView::section {
	background: #181825;
	color: #a6adc8;
	border: none;
	border-bottom: 2px solid #313244;
	padding: 6px 8px;
	font-size: 11px;
	font-weight: 600;
}

QListWidget {
	background: #181825;
	color: #cdd6f4;
	border: 1px solid #2d2d3f;
	border-radius: 4px;
	font-size: 11px;
	outline: none;
}

QListWidget::item {
	padding: 4px 8px;
	border-radius: 3px;
}

QListWidget::item:hover {
	background: #252536;
}

QListWidget::item:selected {
	background: #313244;
	color: #cdd6f4;
}

QComboBox {
	background: #252536;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	padding: 4px 10px;
	font-size: 11px;
	min-height: 24px;
}

QComboBox:hover {
	border-color: #585b70;
}

QComboBox::drop-down {
	border: none;
	width: 20px;
	subcontrol-origin: padding;
	subcontrol-position: top right;
}

QComboBox::down-arrow {
	width: 10px;
	height: 10px;
}

QComboBox QAbstractItemView {
	background: #1e1e2e;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	selection-background-color: #313244;
	padding: 2px;
}

QPushButton {
	background: #313244;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	padding: 5px 14px;
	font-size: 11px;
	min-height: 22px;
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
	border-color: #313244;
}

QPushButton#primaryBtn {
	background: #4a9eff;
	color: #1e1e2e;
	border: none;
	font-weight: bold;
}

QPushButton#primaryBtn:hover {
	background: #6bb5ff;
}

QPushButton#primaryBtn:pressed {
	background: #3a8eef;
}

QPushButton#warningBtn {
	background: #f9e2af;
	color: #6c3f02;
	border: 1px solid #df8e1d;
}

QPushButton#dangerBtn {
	background: #f38ba8;
	color: #1e1e2e;
	border: none;
}

QLabel {
	color: #cdd6f4;
	font-size: 11px;
}

QGroupBox {
	color: #cdd6f4;
	border: 1px solid #2d2d3f;
	border-radius: 6px;
	margin-top: 8px;
	padding-top: 12px;
	font-size: 12px;
	font-weight: 600;
}

QGroupBox::title {
	subcontrol-origin: margin;
	subcontrol-position: top left;
	padding: 0px 6px;
	color: #a6adc8;
}

QSpinBox, QDoubleSpinBox {
	background: #252536;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	padding: 3px 6px;
	font-size: 11px;
	min-height: 22px;
}

QSpinBox:hover, QDoubleSpinBox:hover {
	border-color: #585b70;
}

QSpinBox::up-button, QDoubleSpinBox::up-button,
QSpinBox::down-button, QDoubleSpinBox::down-button {
	background: #313244;
	border: none;
	width: 16px;
	border-radius: 2px;
}

QProgressBar {
	background: #252536;
	border: 1px solid #45475a;
	border-radius: 3px;
	text-align: center;
	color: #cdd6f4;
	font-size: 10px;
	height: 14px;
}

QProgressBar::chunk {
	background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
		stop:0 #4a9eff, stop:1 #6bb5ff);
	border-radius: 2px;
}

QTextEdit, QPlainTextEdit {
	background: #181825;
	color: #cdd6f4;
	border: 1px solid #2d2d3f;
	border-radius: 4px;
	font-size: 11px;
	font-family: "Cascadia Code", "Consolas", "Courier New", monospace;
	selection-background-color: #313244;
}

QScrollBar:vertical {
	background: #181825;
	width: 8px;
	margin: 0;
	border-radius: 4px;
}

QScrollBar::handle:vertical {
	background: #45475a;
	min-height: 20px;
	border-radius: 4px;
}

QScrollBar::handle:vertical:hover {
	background: #585b70;
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
	height: 0;
}

QScrollBar:horizontal {
	background: #181825;
	height: 8px;
	margin: 0;
	border-radius: 4px;
}

QScrollBar::handle:horizontal {
	background: #45475a;
	min-width: 20px;
	border-radius: 4px;
}

QScrollBar::handle:horizontal:hover {
	background: #585b70;
}

QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
	width: 0;
}

QToolTip {
	background: #313244;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	padding: 4px 8px;
	font-size: 11px;
}

QStatusBar {
	background: #181825;
	color: #a6adc8;
	border-top: 1px solid #2d2d3f;
	font-size: 11px;
	padding: 0px 10px;
}

QSplitter::handle {
	background: #2d2d3f;
}

QSplitter::handle:horizontal {
	width: 2px;
}

QSplitter::handle:vertical {
	height: 2px;
}

QTabWidget::pane {
	border: 1px solid #2d2d3f;
	background: #1e1e2e;
	border-radius: 4px;
}

QTabBar::tab {
	background: #181825;
	color: #a6adc8;
	padding: 5px 14px;
	border-bottom: 2px solid transparent;
	font-size: 11px;
}

QTabBar::tab:hover {
	color: #cdd6f4;
}

QTabBar::tab:selected {
	border-bottom: 2px solid #4a9eff;
	color: #4a9eff;
	font-weight: 600;
}

QLineEdit {
	background: #252536;
	color: #cdd6f4;
	border: 1px solid #45475a;
	border-radius: 4px;
	padding: 4px 8px;
	font-size: 11px;
}

QLineEdit:hover {
	border-color: #585b70;
}

QLineEdit:focus {
	border-color: #4a9eff;
}
)";

		qApp->setStyleSheet(QString::fromUtf8(darkTheme));
	}

	inline QString primaryColor() { return QStringLiteral("#4a9eff"); }
	inline QString successColor() { return QStringLiteral("#00c853"); }
	inline QString warningColor() { return QStringLiteral("#f9e2af"); }
	inline QString dangerColor()  { return QStringLiteral("#e81123"); }
	inline QString bgColor()      { return QStringLiteral("#1e1e2e"); }
	inline QString surfaceColor() { return QStringLiteral("#181825"); }
	inline QString textColor()    { return QStringLiteral("#cdd6f4"); }
	inline QString subtextColor() { return QStringLiteral("#a6adc8"); }

	inline QString buttonStyle(const QString& variant = "")
	{
		if (variant == "primary")
			return QStringLiteral("QPushButton { background: #4a9eff; color: #1e1e2e; border: none; "
				"border-radius: 4px; padding: 5px 14px; font-weight: bold; } "
				"QPushButton:hover { background: #6bb5ff; }");
		return QString();
	}

}