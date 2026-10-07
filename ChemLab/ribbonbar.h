#pragma once

#include <QWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QAction>
#include <QAbstractButton>
#include <QMap>
#include <QVector>
#include <QTimer>

class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget* parent = nullptr);

    int addTab(const QString& name);
    void addGroup(int tabIndex, const QString& groupName);
    QAction* addLargeButton(int tabIndex, const QString& groupName,
                            const QString& text, const QIcon& icon = QIcon());
    QAction* addSmallButton(int tabIndex, const QString& groupName,
                            const QString& text, const QIcon& icon = QIcon());
    void addSeparator(int tabIndex, const QString& groupName);

    void addQuickAccessAction(QAction* action);

    int currentTab() const;
    void setCurrentTab(int index);

    QWidget* titleBarWidget() const;
    QWidget* tabBarWidget() const;
    QWidget* qatContainer() const;
    QWidget* leftCorner() const;

    void setIconButton(QAbstractButton* btn);
    void setMinButton(QAbstractButton* btn);
    void setMaxButton(QAbstractButton* btn);
    void setCloseButton(QAbstractButton* btn);

signals:
    void fileMenuClicked();
    void currentTabChanged(int index);

private slots:
    void toggleCollapse();
    void onTabClicked(int index);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void showPopup();
    void hidePopup();

    struct GroupWidget {
        QWidget* container;
        QVBoxLayout* mainLayout;
        QHBoxLayout* buttonLayout;
    };

    QWidget* m_titleRow = nullptr;
    QHBoxLayout* m_titleRowLayout = nullptr;

    QWidget* m_qatContainer;
    QHBoxLayout* m_qatLayout;
    QWidget* m_leftCorner = nullptr;
    QHBoxLayout* m_leftCornerLayout = nullptr;

    QTabBar* m_tabBar = nullptr;
    QStackedWidget* m_tabStack = nullptr;
    QWidget* m_winBtnContainer = nullptr;
    QHBoxLayout* m_winBtnLayout = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;

    QVector<QWidget*> m_tabPages;
    QVector<QMap<QString, GroupWidget>> m_tabGroups;

    bool m_collapsed = false;
    bool m_ribbonPopup = false;
    QTimer* m_showTimer = nullptr;
    QTimer* m_dismissTimer = nullptr;
};