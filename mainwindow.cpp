#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login_scr.h"
#include "reg_scr.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QApplication::exit();
}

void MainWindow::on_pushButton_2_clicked()
{
    reg_scr *wdg = new reg_scr;
    wdg->show();
    close();
}

void MainWindow::on_pushButton_3_clicked()
{
    login_scr *wdg = new login_scr;
    wdg->show();
    close();
}
