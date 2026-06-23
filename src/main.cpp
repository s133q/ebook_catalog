#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include "mainwindow.h"
#include "thememanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setOrganizationName("KHNURE");
    QApplication::setApplicationName("EBookCatalog");
    QApplication::setApplicationVersion("1.0");

    app.setStyle("Fusion");
    ThemeManager::instance().load();

    MainWindow w;
    w.show();

    return app.exec();
}
