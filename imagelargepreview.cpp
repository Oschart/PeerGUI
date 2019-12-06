#include "imagelargepreview.h"
#include "ui_imagelargepreview.h"

ImageLargePreview::ImageLargePreview(std::string path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageLargePreview)
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
