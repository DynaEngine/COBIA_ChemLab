#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>

class LogPanel : public QWidget
{
	Q_OBJECT

public:
	explicit LogPanel(QWidget* parent = nullptr);

	void appendMessage(const QString& msg);

public slots:
	void clearLog();

private:
	QPlainTextEdit* m_logEdit;
	QPushButton* m_clearBtn;
};