#include "viewgrantedimagedialog.h"
#include "ui_viewgrantedimagedialog.h"
#include "Backend/Peer.h"
#include "Backend/ParseUtil.h"
#include "grantedimagepreview.h"

ViewGrantedImageDialog::ViewGrantedImageDialog(std::string _filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewGrantedImageDialog), fileName(_filename)
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
    Image img (Image::readImage((GrantedImages + fileName), sz));
    quota = img.getQuota();
    if (quota){
        Image newCodified (DEF_IMG(sz), img.getContent(), img.getOwner(), quota-1);
        Image::writeImage(GrantedImages+fileName, newCodified.getCodified());
        Image::writeImage("temp.temp", img.getContent());
        GrantedImagePreview* gip = new GrantedImagePreview("temp.temp");
        deleteFile("temp.temp");
        gip->show();
    }
    else {
        GrantedImagePreview* gip = new GrantedImagePreview(GrantedImages + fileName);
        gip->show();
    }
}
