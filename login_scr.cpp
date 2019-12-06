#include "login_scr.h"
#include "reg_scr.h"
#include "ui_login_scr.h"
#include "home.h"
#include "globals.h"
#include <QMessageBox>

login_scr::login_scr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login_scr)
{
    ui->setupUi(this);
}

login_scr::~login_scr()
{
    delete ui;
}

void login_scr::on_pushButton_clicked()
{
    QString username=ui->lineEdit->text();
    QString password=ui->lineEdit_2->text();
    int result = peer.login(username.toUtf8().constData(), password.toUtf8().constData());
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

void login_scr::on_pushButton_2_clicked()
{
    reg_scr *wdg = new reg_scr;
    wdg->show();
    close();
}
