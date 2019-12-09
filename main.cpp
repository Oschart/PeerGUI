#include "mainwindow.h"
#include "Backend/Image.h"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

//    int sz;
//    Image img(Image::readImage("Love_Oscar.jpg", sz));
//    cout << "Love oscar" << endl;
//    cout << img.getQuota() << endl;
//    Image::writeImage("extractedlove.jpg", img.getContent());
//    cout << "the size of getContent() " << img.getContent().size() << endl;
//    cout << "the size of getCodified() " << img.getCodified().size() << endl;

//    Image img2(Image::readImage("codified.jpg", sz));
//    cout << "my codified" << endl;
//    cout << img2.getQuota() << endl;
//    Image::writeImage("extractedcodified.jpg", img2.getContent());
}
