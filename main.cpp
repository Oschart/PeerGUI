#include "mainwindow.h"
#include "Backend/Image.h"
#include <iostream>
#include <QApplication>
#include "Backend/Peer.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto smth = a.exec();
    peer.writeBackQuotaDB();
    return smth;

}
