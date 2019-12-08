#include "grantedimageslist.h"
#include "ui_grantedimageslist.h"
#include "Backend/Peer.h"
#include "viewgrantedimagedialog.h"

GrantedImagesList::GrantedImagesList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrantedImagesList)
{
    ui->setupUi(this);
    ui->listWidget->clear();
    QDir directory(GrantedImages);
    int sz, quota;
    QStringList images = directory.entryList(QStringList() ,QDir::Files);
    foreach(QString filename, images) {
        Image img (Image::readImage((GrantedImages + filename).toStdString(), sz));
        quota = img.getQuota();
        auto item = new QListWidgetItem(filename + " -- " + img.getOwner().c_str() + " -- " + to_string(quota).c_str() + " views remaining");
        item->setSizeHint(QSize(item->sizeHint().width(), 50));
        item->setData(Qt::UserRole, filename);
        ui->listWidget->addItem(item);
    }
}

GrantedImagesList::~GrantedImagesList()
{
    delete ui;
}

void GrantedImagesList::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ViewGrantedImageDialog* dlg = new ViewGrantedImageDialog(item->data(Qt::UserRole).toString().toStdString());
    dlg->show();
    close();
}
