#include <QMessageBox>
#include "reg_scr.h"
#include "ui_reg_scr.h"
#include"home.h"
#include "globals.h"


reg_scr::reg_scr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::reg_scr)
{
    ui->setupUi(this);
}

reg_scr::~reg_scr()
{
    delete ui;
}

void reg_scr::on_pushButton_clicked()
{
    QString username=ui->lineEdit->text();
    QString password=ui->lineEdit_2->text();
    int result = peer.signup(username.toUtf8().constData(), password.toUtf8().constData());
    if (result == 0)
        QMessageBox::information (this, "Login", "Username or password is not correct");
    else if (result == -1)
        QMessageBox::information (this, "Login", "Sorry, cannot connect to the server");
    else {
    home *wdg = new home;
    wdg->show();
    close();
    }
}

