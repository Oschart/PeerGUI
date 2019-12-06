#include "imagelargepreview.h"
#include "ui_imagelargepreview.h"
#include <iostream>
#include <QFileInfo>
#include <QMessageBox>
#include "globals.h"

ImageLargePreview::ImageLargePreview(std::string path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageLargePreview), imagePath(path)
{
    ui->setupUi(this);
    QPixmap pm(path.c_str());
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(pm.scaled(w,h,Qt::KeepAspectRatio));
    ui->label->setAlignment(Qt::AlignCenter);
}

ImageLargePreview::~ImageLargePreview()
{
    delete ui;
}

void ImageLargePreview::on_pushButton_clicked()
{
    close();
}

void ImageLargePreview::on_pushButton_2_clicked()
{
        std::string imageName = QFileInfo(imagePath.c_str()).fileName().toUtf8().constData();
        int res = peer.uploadImagePreview(imageName, imagePath);
        if (res == 1)  QMessageBox::information (this, "Upload", "Upload is successful");
        if (res == 0)  QMessageBox::information (this, "Upload", "Upload is unsuccessful");
        if (res == -1)  QMessageBox::information (this, "Upload", "Cannot connect to server");


}
