#include "offernewquotadialog.h"
#include "ui_offernewquotadialog.h"
#include "globals.h"
#include <QMessageBox>

OfferNewQuotaDialog::OfferNewQuotaDialog(string _user, string _img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OfferNewQuotaDialog), user(_user), imageName(_img)
{
    ui->setupUi(this);
    ui->label->setText(std::string("Would you like to offer " + user + " a new quota for the image " + imageName + "?").c_str());
}

OfferNewQuotaDialog::~OfferNewQuotaDialog()
{
    delete ui;
}

void OfferNewQuotaDialog::on_buttonBox_accepted()
{
    int quota = ui->lineEdit->text().toInt();
    int res = peer.setImageQuota(user, imageName, quota);
    if (res == -1) QMessageBox::information (this, "Setting Quota", "An error occurred. Try again");
    else if (res == 1){
        QMessageBox::information (this, "Setting Quota", "Successfully updated the quota");
    }
    else {
        QMessageBox::information (this, "Setting Quota", "The quota will be updated once the user is online");
    }

}
