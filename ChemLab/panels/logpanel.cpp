#include "logpanel.h"

#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>

LogPanel::LogPanel(QWidget* parent)
	: QWidget(parent)
	, m_logEdit(nullptr)
	, m_clearBtn(nullptr)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);

	m_logEdit = new QPlainTextEdit(this);
	m_logEdit->setReadOnly(true);
	m_logEdit->setMaximumBlockCount(5000);
	layout->addWidget(m_logEdit);

	auto* bottomLayout = new QHBoxLayout();
	bottomLayout->setContentsMargins(4, 2, 4, 2);

	m_clearBtn = new QPushButton(QStringLiteral("Clear"), this);
	m_clearBtn->setFixedHeight(20);
	m_clearBtn->setFlat(true);
	m_clearBtn->setStyleSheet(QStringLiteral(
		"QPushButton { color:#a6adc8; border:1px solid #3e3e55; border-radius:3px; padding:0 8px; font-size:10px; }"
		"QPushButton:hover { background:#313244; color:#cdd6f4; }"));
	bottomLayout->addWidget(m_clearBtn);
	bottomLayout->addStretch();

	layout->addLayout(bottomLayout);

	connect(m_clearBtn, &QPushButton::clicked, this, &LogPanel::clearLog);
}

void LogPanel::appendMessage(const QString& msg)
{
	QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
	m_logEdit->appendPlainText(QStringLiteral("[%1] %2").arg(timestamp, msg));
}

void LogPanel::clearLog()
{
	m_logEdit->clear();
}