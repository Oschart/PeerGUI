#include "approverequestdialog.h"
#include "ui_approverequestdialog.h"
#include <iostream>

ApproveRequestDialog::ApproveRequestDialog(std::string _req, std::string _img, int _q, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApproveRequestDialog), requester(_req), imageName(_img), quota(_q)

{
    ui->setupUi(this);

    ui->label->setText(std::string("Do you agree to give " + requester + " a quota of " + std::to_string(quota) + " to your image " + imageName + "?").c_str());
}

ApproveRequestDialog::~ApproveRequestDialog()
{
    delete ui;
}

void ApproveRequestDialog::on_pushButton_2_clicked()
{
    close();
}

void ApproveRequestDialog::on_pushButton_clicked()
{
    std::cout << "Will call approve" << std::endl;
    close();
}
