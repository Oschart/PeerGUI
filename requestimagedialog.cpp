#include "requestimagedialog.h"
#include "ui_requestimagedialog.h"

RequestImageDialog::RequestImageDialog(std::string _owner, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RequestImageDialog), owner(_owner)
{
    ui->setupUi(this);
    ui->label->setText(std::string("This image is owned by " + owner + ". Would you like to request it?").c_str());
}

RequestImageDialog::~RequestImageDialog()
{
    delete ui;
}
