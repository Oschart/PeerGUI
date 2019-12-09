#include "grantedimageslist.h"
#include "ui_grantedimageslist.h"
#include "Backend/Peer.h"
#include "viewgrantedimagedialog.h"
#include "globals.h"

GrantedImagesList::GrantedImagesList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrantedImagesList)
{
    ui->setupUi(this);
    ui->listWidget->clear();
    QDir directory(peer.GrantedImages.c_str());
    int sz, quota;
    QStringList images = directory.entryList(QStringList() ,QDir::Files);
    foreach(QString filename, images) {
        Image img (Image::readImage((peer.GrantedImages.c_str() + filename).toStdString(), sz));
        quota = img.getQuota();
        auto nameWithoutUser = filename.toStdString();
        removeUserfromName(nameWithoutUser);
        auto item = new QListWidgetItem((nameWithoutUser + " -- " + img.getOwner().c_str() + " -- " + to_string(quota).c_str() + " views remaining").c_str());
        item->setSizeHint(QSize(item->sizeHint().width(), 50));
        item->setData(Qt::UserRole, filename);
        item->setData(Qt::DecorationPropertyRole, img.getOwner().c_str());
        ui->listWidget->addItem(item);
    }
}

GrantedImagesList::~GrantedImagesList()
{
    delete ui;
}

void GrantedImagesList::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ViewGrantedImageDialog* dlg = new ViewGrantedImageDialog(item->data(Qt::DecorationPropertyRole).toString().toStdString(), item->data(Qt::UserRole).toString().toStdString());
    dlg->show();
    close();
}
