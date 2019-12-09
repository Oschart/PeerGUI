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
//    int sz, sz2;

//    Image someimg(DEF_IMG(sz), Image::readImage("Love.jpg", sz2), "Fadi", 11);
//    Image::writeImage("validCodified.jpg", someimg.getCodified());


//    string codifiedPath = "validCodified.jpg";
//    vector<uint8_t> cod = Image::readImage(codifiedPath, sz);

//    Image img = Image(cod);
//    img = Image(DEF_IMG(sz), img.getContent(), img.getOwner(), 19);

//    // Write back codified image
//    Image::writeImage("oscarCodified.jpg", img.getCodified());



//    Image img2(Image::readImage("validCodified.jpg", sz));
//    cout << "Mine" << endl;
//    cout << img2.getQuota() << endl;
//    Image::writeImage("myextracted.jpg", img2.getContent());
//    cout << "the size of getContent() " << img2.getContent().size() << endl;
//    cout << "the size of getCodified() " << img2.getCodified().size() << endl;

//    Image img3(Image::readImage("oscarCodified.jpg", sz));
//    cout << "Oscar" << endl;
//    cout << img3.getQuota() << endl;
//    Image::writeImage("oscarextracted.jpg", img3.getContent());
//    cout << "the size of getContent() " << img3.getContent().size() << endl;
//    cout << "the size of getCodified() " << img3.getCodified().size() << endl;

//    Image img2(Image::readImage("codified.jpg", sz));
//    cout << "my codified" << endl;
//    cout << img2.getQuota() << endl;
//    Image::writeImage("extractedcodified.jpg", img2.getContent());
}
