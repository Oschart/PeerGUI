#include "visitprofile.h"
#include "ui_visitprofile.h"

visitProfile::visitProfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::visitProfile)
{
    ui->setupUi(this);
}

visitProfile::~visitProfile()
{
    delete ui;
}
