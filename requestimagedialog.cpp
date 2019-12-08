#include "requestimagedialog.h"
#include "ui_requestimagedialog.h"
#include "globals.h"
#include <QMessageBox>

RequestImageDialog::RequestImageDialog(std::string _owner, std::string _img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RequestImageDialog), owner(_owner), imageName(_img)
{
    ui->setupUi(this);
    ui->label->setText(std::string("This image is owned by " + owner + ". Would you like to request it?").c_str());
}

RequestImageDialog::~RequestImageDialog()
{
    delete ui;
}

void RequestImageDialog::on_buttonBox_accepted()
{
    int quota = stoi(ui->lineEdit->text().toStdString());

    int res = peer.requestImage(owner, imageName, quota);
    if (res == -1) QMessageBox::information (this, "Request", "An error occurred. Try again");
    else if (res == 1){
        QMessageBox::information (this, "Request", "Your request successfully reached the owner");
    }
    else {
        QMessageBox::information (this, "Request", "Your request will arrive to the owner as soon as they are online");
    }
    close();
}
