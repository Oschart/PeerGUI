#include "home.h"
#include "ui_home.h"
#include "browser.h"
#include "visitprofile.h"
#include "imagelargepreview.h"
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
#include "globals.h"
#include "Backend/ParseUtil.h"
//using namespace std;


home::home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(2);


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
    QString filePath = QFileDialog::getOpenFileName(this, "Upload an image");
    QString filename = QFileInfo(filePath).fileName();
    std::cout << QFile::copy(filePath, MyImages + filename) << std::endl;
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
        ui->listView_2->addItem(new QListWidgetItem(QIcon(MyImages + filename), filename));
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

    ui->listView->setViewMode(QListWidget::IconMode);

    ui->listView->setIconSize(QSize(50, 50));

    ui->listView->setResizeMode((QListWidget::Adjust));
    ui->listView->clear();

    int res =peer.getPreviews();
    if (res == -1)  QMessageBox::information (this, "Previews", "Could not connect to the server");
    else  {
        ui->stackedWidget->setCurrentIndex(0);
        QDir directory(PREVIEWS);
        QStringList images = directory.entryList(QStringList() ,QDir::Files);
        foreach(QString filename, images) {
            string orig = filename.toUtf8().constData();
            string user = removeUserfromName(orig);
            ui->listView->addItem(new QListWidgetItem(QIcon(PREVIEWS + filename), (user + ("\n") + orig).c_str()));
        }
        peer.clearTempImages();
    }

//    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/1.jpeg"), "landscape\nEslam"));
//    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/2.jpeg"), "nature\nFadi"));
//    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/3.jpeg"), "charming\nOscar"));
//    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/4.jpeg"), "cool\nRagab"));
//    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/DS_GUI1/imgs/5.jpeg"), "Nice\nAmr"));

}



void home::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}

void home::on_listView_2_itemPressed(QListWidgetItem *item)
{
    ImageLargePreview * pr = new ImageLargePreview((MyImages + item->text()).toUtf8().constData());
    pr->show();

}
