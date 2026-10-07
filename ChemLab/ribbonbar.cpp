#include "ribbonbar.h"

#include <QStyle>
#include <QMouseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QPainter>

static const char* kRibbonStyle = R"(
    RibbonBar {
        border: none;
        background: #11111b;
    }

    RibbonBar #ribbonStack {
        border: none;
        padding: 0px;
        margin: 0px;
        background: #181825;
    }

    RibbonBar QTabBar {
        background: #181825;
        border: none;
    }
    RibbonBar QTabBar::tab {
        background: transparent;
        color: #a6adc8;
        padding: 6px 18px;
        margin: 0px 0px;
        border: none;
        border-bottom: 2px solid transparent;
        font-size: 12px;
        font-family: "Microsoft YaHei", sans-serif;
    }
    RibbonBar QTabBar::tab:hover {
        color: #cdd6f4;
        border-bottom: 2px solid #45475a;
    }
    RibbonBar QTabBar::tab:selected {
        border-bottom: 2px solid #4a9eff;
        font-weight: 600;
        color: #cdd6f4;
    }

    RibbonBar .GroupSeparator {
        background: #2d2d3f;
        margin: 2px 2px;
    }

    RibbonBar .GroupTitle {
        color: #a6adc8;
        font-size: 10px;
        padding: 0px 4px 4px 4px;
        border-bottom: 1px solid #2d2d3f;
        font-family: "Microsoft YaHei", sans-serif;
    }

    RibbonBar .RibbonLargeBtn {
        border: 1px solid transparent;
        border-radius: 4px;
        padding: 4px 6px;
        background: transparent;
        font-size: 11px;
        color: #cdd6f4;
        font-family: "Microsoft YaHei", sans-serif;
    }
    RibbonBar .RibbonLargeBtn:hover {
        background: #313244;
        border-color: #45475a;
        color: #ffffff;
    }
    RibbonBar .RibbonLargeBtn:pressed {
        background: #45475a;
        border-color: #585b70;
    }

    RibbonBar .RibbonSmallBtn {
        border: 1px solid transparent;
        border-radius: 3px;
        padding: 2px 10px;
        background: transparent;
        font-size: 11px;
        color: #cdd6f4;
        text-align: left;
        font-family: "Microsoft YaHei", sans-serif;
    }
    RibbonBar .RibbonSmallBtn:hover {
        background: #313244;
        border-color: #45475a;
        color: #ffffff;
    }
    RibbonBar .RibbonSmallBtn:pressed {
        background: #45475a;
        border-color: #585b70;
    }

    RibbonBar .InlineSep {
        background: #181825;
        border: none;
        margin: 4px 3px;
    }

    RibbonBar #icon-button {
        background: transparent;
        border: none;
        border-radius: 0px;
        qproperty-iconSize: 16px;
        qproperty-iconColor: #cdd6f4;
    }
    RibbonBar #icon-button:hover {
        background: #313244;
    }

    RibbonBar #min-button,
    RibbonBar #max-button,
    RibbonBar #close-button {
        background: transparent;
        border: none;
        border-radius: 0px;
        qproperty-iconSize: 12px;
        qproperty-iconColor: #cdd6f4;
    }
    RibbonBar #min-button:hover,
    RibbonBar #max-button:hover {
        background: #313244;
    }
    RibbonBar #close-button:hover {
        background: #e81123;
    }
    RibbonBar #close-button:hover:pressed {
        background: #bf0f1d;
    }
)";

static const char* kQatBtnStyle = R"(
    QToolButton {
        background: transparent;
        border: 1px solid transparent;
        border-radius: 4px;
    }
    QToolButton:hover {
        background: #313244;
        border: 1px solid #45475a;
    }
    QToolButton:pressed {
        background: #45475a;
    }
)";

RibbonBar::RibbonBar(QWidget* parent)
    : QWidget(parent)
    , m_qatContainer(nullptr)
    , m_qatLayout(nullptr)
    , m_tabBar(nullptr)
    , m_tabStack(nullptr)
{
    setupUi();
}

void RibbonBar::setupUi()
{
    setObjectName(QStringLiteral("RibbonBar"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setStyleSheet(kRibbonStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    m_mainLayout = mainLayout;

    m_titleRow = new QWidget(this);
    m_titleRow->setObjectName(QStringLiteral("ribbonTitleRow"));
    m_titleRow->setFixedHeight(28);
    m_titleRow->setStyleSheet(QStringLiteral(
        "#ribbonTitleRow { background: #11111b; border: none; }"));
    m_titleRowLayout = new QHBoxLayout(m_titleRow);
    m_titleRowLayout->setContentsMargins(0, 0, 0, 0);
    m_titleRowLayout->setSpacing(0);

    m_qatContainer = new QWidget(m_titleRow);
    m_qatContainer->setObjectName(QStringLiteral("qatContainer"));
    m_qatLayout = new QHBoxLayout(m_qatContainer);
    m_qatLayout->setContentsMargins(0, 0, 0, 0);
    m_qatLayout->setSpacing(2);

    m_leftCorner = new QWidget(m_titleRow);
    m_leftCornerLayout = new QHBoxLayout(m_leftCorner);
    m_leftCornerLayout->setContentsMargins(0, 0, 4, 0);
    m_leftCornerLayout->setSpacing(0);
    m_leftCornerLayout->addWidget(m_qatContainer);
    m_titleRowLayout->addWidget(m_leftCorner);

    m_tabBar = new QTabBar(m_titleRow);
    m_tabBar->setExpanding(false);
    m_tabBar->setUsesScrollButtons(false);
    m_tabBar->setCursor(Qt::PointingHandCursor);
    m_tabBar->setDrawBase(false);
    m_tabBar->setAutoFillBackground(true);
    {
        QPalette pal = m_tabBar->palette();
        pal.setColor(QPalette::Window, QColor(0x18, 0x18, 0x25));
        m_tabBar->setPalette(pal);
    }
    m_titleRowLayout->addWidget(m_tabBar);
    m_titleRowLayout->addSpacing(6);
    m_titleRowLayout->addStretch(1);

    m_winBtnContainer = new QWidget(m_titleRow);
    m_winBtnContainer->setObjectName(QStringLiteral("winBtnContainer"));
    m_winBtnLayout = new QHBoxLayout(m_winBtnContainer);
    m_winBtnLayout->setContentsMargins(0, 0, 0, 0);
    m_winBtnLayout->setSpacing(0);
    m_titleRowLayout->addWidget(m_winBtnContainer);

    mainLayout->addWidget(m_titleRow);

    m_tabStack = new QStackedWidget(this);
    m_tabStack->setObjectName(QStringLiteral("ribbonStack"));
    m_tabStack->setAutoFillBackground(true);
    {
        QPalette pal = m_tabStack->palette();
        pal.setColor(QPalette::Window, QColor(0x18, 0x18, 0x25));
        pal.setColor(QPalette::Base, QColor(0x18, 0x18, 0x25));
        m_tabStack->setPalette(pal);
    }
    mainLayout->addWidget(m_tabStack);

    QFrame* bottomLine = new QFrame(this);
    bottomLine->setFrameShape(QFrame::HLine);
    bottomLine->setFrameShadow(QFrame::Plain);
    bottomLine->setFixedHeight(1);
    bottomLine->setStyleSheet(QStringLiteral("QFrame { color: #2d2d3f; }"));
    mainLayout->addWidget(bottomLine);

    connect(m_tabBar, &QTabBar::tabBarDoubleClicked,
            this, &RibbonBar::toggleCollapse);
    connect(m_tabBar, &QTabBar::tabBarClicked,
            this, &RibbonBar::onTabClicked);
    connect(m_tabBar, &QTabBar::currentChanged,
            this, &RibbonBar::currentTabChanged);
    connect(m_tabBar, &QTabBar::currentChanged,
            m_tabStack, &QStackedWidget::setCurrentIndex);

    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, &RibbonBar::showPopup);

    m_dismissTimer = new QTimer(this);
    m_dismissTimer->setSingleShot(true);
    connect(m_dismissTimer, &QTimer::timeout, this, &RibbonBar::hidePopup);
}

void RibbonBar::toggleCollapse()
{
    m_showTimer->stop();
    m_dismissTimer->stop();

    if (m_collapsed) {
        m_collapsed = false;
        hidePopup();
        if (m_tabStack->parent() != this) {
            m_tabStack->hide();
            m_tabStack->setParent(this);
            m_tabStack->setWindowFlags(Qt::Widget);
            m_mainLayout->addWidget(m_tabStack);
        }
        m_tabStack->setVisible(true);
    } else {
        m_collapsed = true;
        hidePopup();
        m_tabStack->setVisible(false);
    }
    updateGeometry();
    adjustSize();
}

void RibbonBar::onTabClicked(int index)
{
    if (!m_collapsed)
        return;

    m_showTimer->stop();
    m_tabBar->setCurrentIndex(index);

    if (!m_ribbonPopup) {
        m_ribbonPopup = true;
        m_showTimer->start(250);
    }
    m_tabStack->setCurrentIndex(index);
}

void RibbonBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        toggleCollapse();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void RibbonBar::enterEvent(QEnterEvent* event)
{
    m_dismissTimer->stop();
    QWidget::enterEvent(event);
}

void RibbonBar::leaveEvent(QEvent* event)
{
    if (m_ribbonPopup)
        m_dismissTimer->start(200);
    QWidget::leaveEvent(event);
}

bool RibbonBar::eventFilter(QObject* obj, QEvent* event)
{
    if (!m_ribbonPopup)
        return QWidget::eventFilter(obj, event);

    if (obj == m_tabStack) {
        if (event->type() == QEvent::Enter) {
            m_dismissTimer->stop();
        } else if (event->type() == QEvent::Leave) {
            m_dismissTimer->start(200);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void RibbonBar::showPopup()
{
    m_mainLayout->removeWidget(m_tabStack);
    m_tabStack->setParent(nullptr);
    m_tabStack->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    m_tabStack->installEventFilter(this);
    m_tabStack->setStyleSheet(QStringLiteral(
        "#ribbonStack { background: #181825; }"
        "#ribbonStack > QWidget { background: #181825; }"
        ".RibbonLargeBtn { border:1px solid transparent; border-radius:4px; "
        "padding:4px 6px; background:transparent; font-size:11px; color:#cdd6f4; }"
        ".RibbonLargeBtn:hover { background:#313244; border-color:#45475a; color:#fff; }"
        ".RibbonLargeBtn:pressed { background:#45475a; border-color:#585b70; }"
        ".RibbonSmallBtn { border:1px solid transparent; border-radius:3px; "
        "padding:2px 10px; background:transparent; font-size:11px; color:#cdd6f4; text-align:left; }"
        ".RibbonSmallBtn:hover { background:#313244; border-color:#45475a; color:#fff; }"
        ".RibbonSmallBtn:pressed { background:#45475a; border-color:#585b70; }"
        ".GroupSeparator { background:#2d2d3f; margin:2px 2px; }"
        ".GroupTitle { color:#a6adc8; font-size:10px; padding:0 4px 4px 4px; "
        "border-bottom:1px solid #2d2d3f; }"
        ".InlineSep { background:#181825; border:none; margin:4px 3px; }"));
    QPoint pos = m_titleRow->mapToGlobal(QPoint(0, m_titleRow->height()));
    m_tabStack->move(pos);
    m_tabStack->show();
}

void RibbonBar::hidePopup()
{
    if (!m_ribbonPopup)
        return;
    m_ribbonPopup = false;
    m_showTimer->stop();
    m_dismissTimer->stop();
    m_tabStack->removeEventFilter(this);
    m_tabStack->setStyleSheet(QString());
    m_tabStack->hide();
    m_tabStack->setParent(this);
    m_tabStack->setWindowFlags(Qt::Widget);
    m_mainLayout->addWidget(m_tabStack);
}

int RibbonBar::addTab(const QString& name)
{
    QWidget* page = new QWidget(this);
    QHBoxLayout* pageLayout = new QHBoxLayout(page);
    pageLayout->setContentsMargins(6, 4, 6, 6);
    pageLayout->setSpacing(0);
    pageLayout->addStretch(1);

    int idx = m_tabBar->addTab(name);
    m_tabStack->addWidget(page);
    m_tabPages.append(page);
    m_tabGroups.append(QMap<QString, GroupWidget>());
    return idx;
}

void RibbonBar::addGroup(int tabIndex, const QString& groupName)
{
    if (tabIndex < 0 || tabIndex >= m_tabPages.size())
        return;

    QWidget* page = m_tabPages[tabIndex];
    QHBoxLayout* pageLayout = qobject_cast<QHBoxLayout*>(page->layout());
    if (!pageLayout)
        return;

    if (m_tabGroups[tabIndex].contains(groupName))
        return;

    int stretchCount = 0;
    for (int i = pageLayout->count() - 1; i >= 0; --i) {
        if (pageLayout->itemAt(i)->widget() == nullptr)
            stretchCount++;
        else
            break;
    }
    while (stretchCount > 1) {
        pageLayout->takeAt(pageLayout->count() - 1);
        stretchCount--;
    }

    QFrame* separator = new QFrame(page);
    separator->setFixedWidth(1);
    separator->setObjectName(QStringLiteral("GroupSeparator"));
    separator->setProperty("class", QStringLiteral("GroupSeparator"));
    pageLayout->insertWidget(pageLayout->count() - stretchCount, separator);

    QWidget* container = new QWidget(page);
    container->setFixedWidth(200);
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(6, 2, 6, 2);
    mainLayout->setSpacing(2);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(3);
    mainLayout->addLayout(buttonLayout);

    QLabel* titleLabel = new QLabel(groupName, container);
    titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    titleLabel->setObjectName(QStringLiteral("GroupTitle"));
    titleLabel->setProperty("class", QStringLiteral("GroupTitle"));
    titleLabel->setFixedHeight(18);
    mainLayout->addWidget(titleLabel);

    pageLayout->insertWidget(pageLayout->count() - stretchCount, container);

    pageLayout->addStretch(1);

    GroupWidget gw;
    gw.container = container;
    gw.mainLayout = mainLayout;
    gw.buttonLayout = buttonLayout;
    m_tabGroups[tabIndex][groupName] = gw;
}

QAction* RibbonBar::addLargeButton(int tabIndex, const QString& groupName,
                                    const QString& text, const QIcon& icon)
{
    if (tabIndex < 0 || tabIndex >= m_tabGroups.size())
        return nullptr;

    auto it = m_tabGroups[tabIndex].find(groupName);
    if (it == m_tabGroups[tabIndex].end())
        return nullptr;

    QToolButton* btn = new QToolButton(it->container);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setIconSize(QSize(32, 32));
    btn->setMinimumSize(QSize(56, 60));
    btn->setMaximumSize(QSize(80, 60));
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setObjectName(QStringLiteral("RibbonLargeBtn"));
    btn->setProperty("class", QStringLiteral("RibbonLargeBtn"));

    QAction* action = new QAction(icon, text, btn);
    btn->setDefaultAction(action);

    if (!icon.isNull()) {
        QPixmap pm(32, 32);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        icon.paint(&p, 0, 0, 32, 32);
        p.end();
        btn->setIcon(QIcon(pm));
    }

    it->buttonLayout->addWidget(btn);
    return action;
}

QAction* RibbonBar::addSmallButton(int tabIndex, const QString& groupName,
                                    const QString& text, const QIcon& icon)
{
    if (tabIndex < 0 || tabIndex >= m_tabGroups.size())
        return nullptr;

    auto it = m_tabGroups[tabIndex].find(groupName);
    if (it == m_tabGroups[tabIndex].end())
        return nullptr;

    QToolButton* btn = new QToolButton(it->container);
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setIconSize(QSize(16, 16));
    btn->setMinimumHeight(24);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setObjectName(QStringLiteral("RibbonSmallBtn"));
    btn->setProperty("class", QStringLiteral("RibbonSmallBtn"));

    QAction* action = new QAction(icon, text, btn);
    btn->setDefaultAction(action);

    if (!icon.isNull()) {
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        icon.paint(&p, 0, 0, 16, 16);
        p.end();
        btn->setIcon(QIcon(pm));
    }

    it->buttonLayout->addWidget(btn);
    return action;
}

void RibbonBar::addSeparator(int tabIndex, const QString& groupName)
{
    if (tabIndex < 0 || tabIndex >= m_tabGroups.size())
        return;

    auto it = m_tabGroups[tabIndex].find(groupName);
    if (it == m_tabGroups[tabIndex].end())
        return;

    QFrame* sep = new QFrame(it->container);
    sep->setFixedWidth(1);
    sep->setObjectName(QStringLiteral("InlineSep"));
    sep->setProperty("class", QStringLiteral("InlineSep"));
    it->buttonLayout->addWidget(sep);
}

void RibbonBar::addQuickAccessAction(QAction* action)
{
    if (!action || !m_qatLayout)
        return;

    QToolButton* btn = new QToolButton(m_qatContainer);
    btn->setDefaultAction(action);
    btn->setIconSize(QSize(14, 14));
    btn->setFixedSize(24, 24);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(kQatBtnStyle);
    m_qatLayout->addWidget(btn);
}

int RibbonBar::currentTab() const
{
    return m_tabBar->currentIndex();
}

void RibbonBar::setCurrentTab(int index)
{
    m_tabBar->setCurrentIndex(index);
    m_tabStack->setCurrentIndex(index);
}

QWidget* RibbonBar::titleBarWidget() const
{
    return m_titleRow;
}

QWidget* RibbonBar::tabBarWidget() const
{
    return m_tabBar;
}

QWidget* RibbonBar::qatContainer() const
{
    return m_qatContainer;
}

QWidget* RibbonBar::leftCorner() const
{
    return m_leftCorner;
}

void RibbonBar::setIconButton(QAbstractButton* btn)
{
    if (!btn || !m_leftCornerLayout)
        return;
    btn->setFixedSize(28, 28);
    btn->setCursor(Qt::PointingHandCursor);
    m_leftCornerLayout->insertWidget(0, btn);
}

void RibbonBar::setMinButton(QAbstractButton* btn)
{
    if (!m_winBtnLayout || !btn)
        return;
    btn->setFixedSize(36, 28);
    btn->setCursor(Qt::PointingHandCursor);
    m_winBtnLayout->addWidget(btn);
}

void RibbonBar::setMaxButton(QAbstractButton* btn)
{
    if (!m_winBtnLayout || !btn)
        return;
    btn->setFixedSize(36, 28);
    btn->setCursor(Qt::PointingHandCursor);
    m_winBtnLayout->addWidget(btn);
}

void RibbonBar::setCloseButton(QAbstractButton* btn)
{
    if (!m_winBtnLayout || !btn)
        return;
    btn->setFixedSize(36, 28);
    btn->setCursor(Qt::PointingHandCursor);
    m_winBtnLayout->addWidget(btn);
}