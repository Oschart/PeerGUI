#include "grantedimagepreview.h"
#include "ui_grantedimagepreview.h"

GrantedImagePreview::GrantedImagePreview(std::string _path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrantedImagePreview), path(_path)
{
    ui->setupUi(this);
    QPixmap pm(path.c_str());
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(pm.scaled(w,h,Qt::KeepAspectRatio));
    ui->label->setAlignment(Qt::AlignCenter);
}

GrantedImagePreview::~GrantedImagePreview()
{
    delete ui;
}
