#include "viewgrantedimagedialog.h"
#include "ui_viewgrantedimagedialog.h"
#include "requestimagedialog.h"
#include "Backend/Peer.h"
#include "Backend/ParseUtil.h"
#include "grantedimagepreview.h"
#include "globals.h"

ViewGrantedImageDialog::ViewGrantedImageDialog(std::string _own, std::string _filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewGrantedImageDialog), fileName(_filename), owner(_own)
{
    ui->setupUi(this);
}

ViewGrantedImageDialog::~ViewGrantedImageDialog()
{
    delete ui;
}

void ViewGrantedImageDialog::on_buttonBox_rejected()
{
    close();
}

void ViewGrantedImageDialog::on_buttonBox_accepted()
{
    int sz, quota;
    Image img (Image::readImage((peer.GrantedImages + fileName), sz));
    quota = img.getQuota();
    if (quota){
        cout << "The remaining quota is " << quota << endl;
        Image newCodified (DEF_IMG(sz), img.getContent(), img.getOwner(), quota-1);
        Image::writeImage(peer.GrantedImages+fileName, newCodified.getCodified());
        Image::writeImage("temp.temp", img.getContent());
        GrantedImagePreview* gip = new GrantedImagePreview("temp.temp");
        //deleteFile("temp.temp");
        gip->show();
        peer.notifyView(fileName);
    }
    else {
        GrantedImagePreview* gip = new GrantedImagePreview(peer.GrantedImages + fileName);
        gip->show();
    }
}

void ViewGrantedImageDialog::on_pushButton_clicked()
{
    RequestImageDialog* rid = new RequestImageDialog(owner, fileName, false);
    rid->show();
}
