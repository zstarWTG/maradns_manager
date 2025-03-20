#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"

#include "TxtHandler.h"
#include <QDebug>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("none");
    QCoreApplication::setOrganizationDomain("a.b");
    QCoreApplication::setApplicationName("dns_manager");
    QApplication::setWindowIcon(QIcon(":/res/icon.png"));

    QTranslator trans;
    if (trans.load("./" + QLocale().name() + ".qm")) {
        QCoreApplication::installTranslator(&trans);
    }

    mainwindow w;
    w.show();

    return QApplication::exec();
}