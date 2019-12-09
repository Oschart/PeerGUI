#include "approverequestdialog.h"
#include "ui_approverequestdialog.h"
#include <iostream>
#include "globals.h"

ApproveRequestDialog::ApproveRequestDialog(bool _new, std::string _req, std::string _img, int _q, int _ind, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApproveRequestDialog), requester(_req), imageName(_img), quota(_q), newImage(_new), index(_ind)

{
    ui->setupUi(this);
    std::string msg = newImage? "Do you agree to give " + requester + " a quota of " + std::to_string(quota) + " to your image " + imageName + "?":
                                "Do you agree to update " + requester + " to a quota of " + std::to_string(quota) + " to your granted image " + imageName + "?";
    ui->label->setText(msg.c_str());
}

ApproveRequestDialog::~ApproveRequestDialog()
{
    delete ui;
}

void ApproveRequestDialog::on_pushButton_2_clicked()
{
    peer.answerImageRequest(index, 0);
    close();
}

void ApproveRequestDialog::on_pushButton_clicked()
{
    peer.answerImageRequest(index, 1);
    close();
}
