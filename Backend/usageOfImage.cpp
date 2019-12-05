#include <iostream>
#include <vector>
#include "Image.h"

using namespace std;

int main() {
    int sz1, sz2;
    Image img (Image::readImage("SPACE.jpg", sz1), Image::readImage("skull2.png", sz2), "fadiadel2010", 992933243);
    auto codified = img.getCodified();
    Image::writeImage("codified.jpg", codified);
    Image img2 (codified);
    Image::writeImage("hidden.png", img2.getContent());
    cout << img2.getOwner() << endl;
    cout << img2.getQuota() << endl;


}
