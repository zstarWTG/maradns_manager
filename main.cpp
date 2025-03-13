#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"

#include "TxtHandler.h"
#include <QDebug>
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("none");
    QCoreApplication::setOrganizationDomain("a.b");
    QCoreApplication::setApplicationName("dns_manager");

    mainwindow w;
    w.show();

    return QApplication::exec();
}