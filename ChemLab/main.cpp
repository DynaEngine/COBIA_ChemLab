#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include <QDir>
#include "mainwindow.h"

#include <COBIA.h>

int main(int argc, char* argv[])
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    COBIA::capeInitialize();

    QApplication app(argc, argv);

    QDir::setCurrent(QCoreApplication::applicationDirPath());

    app.setStyle(QStyleFactory::create("Fusion"));

    QFont defaultFont("Microsoft YaHei", 9);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(defaultFont);

    app.setStyleSheet(R"(
        * {
            font-family: "Microsoft YaHei", sans-serif;
        }
        QToolTip {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 11px;
        }
    )");

    app.setApplicationName("ChemLab");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("ChemLab");

    MainWindow window;

    window.showMaximized();

    return app.exec();
}