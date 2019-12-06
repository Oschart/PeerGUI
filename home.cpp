#include "home.h"
#include "ui_home.h"
#include "browser.h"
#include "visitprofile.h"
#include <QTextStream>
#include <QMessageBox>
#include <iostream>
#include <QFile>
 #include <QStackedWidget>
#include <QFileSystemModel>
#include <QWidget>
#include <QListWidget>
#include <QListView>
#include <QFileDialog>
#include <QDebug>
#include "Backend/Peer.h"
//using namespace std;


home::home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);

}

home::~home()
{
    delete ui;
}

void home::on_pushButton_3_clicked()
{
    QApplication::exit();
}

void home::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Upload an img");
    QDir d = QFileInfo(filePath).absoluteDir();
    QString absolute=d.absolutePath();
    qDebug() << absolute;

}

void home::on_pushButton_2_clicked()
{

    ui->stackedWidget->setCurrentIndex(3);
    ui->listView_2->setViewMode(QListWidget::IconMode);
    ui->listView_2->setIconSize(QSize(50, 50));
    ui->listView_2->setResizeMode((QListWidget::Adjust));
    ui->listView_2->clear();
    QDir directory(MyImages);
    QStringList images = directory.entryList(QStringList() ,QDir::Files);
    foreach(QString filename, images) {
        ui->listView_2->addItem(new QListWidgetItem(QIcon(string(string(MyImages) + "/").c_str() + filename), filename));
    }

}


void home::on_pushButton_8_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}



void home::on_pushButton_13_clicked()
{
    QString username = ui->lineEdit_2->text();

    QFile inputFile("/home/wan/DS_GUI1/profileName.txt");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          if(username == line) {
              visitProfile *wdg = new visitProfile;
              wdg->show();
              //close()
          }
       }
       inputFile.close();
    }


}


void home::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

    ui->listView->setViewMode(QListWidget::IconMode);

    ui->listView->setIconSize(QSize(50, 50));

    ui->listView->setResizeMode((QListWidget::Adjust));

    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/1.jpeg"), "landscape\nEslam"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/2.jpeg"), "nature\nFadi"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/3.jpeg"), "charming\nOscar"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/4.jpeg"), "cool\nRagab"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/5.jpeg"), "Nice\nAmr"));

}



void home::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}
