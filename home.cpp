#include "home.h"
#include "ui_home.h"
#include "browser.h"
#include "visitprofile.h"
#include "imagelargepreview.h"
#include "requestimagedialog.h"
#include "approverequestdialog.h"
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
//    QString username = ui->lineEdit_2->text();

//    QFile inputFile("/home/wan/DS_GUI1/profileName.txt");
//    if (inputFile.open(QIODevice::ReadOnly))
//    {
//       QTextStream in(&inputFile);
//       while (!in.atEnd())
//       {
//          QString line = in.readLine();
//          if(username == line) {
//              visitProfile *wdg = new visitProfile;
//              wdg->show();
//              //close()
//          }
//       }
//       inputFile.close();
//    }

    string username = ui->lineEdit_2->text().toUtf8().constData();
    if (username != ""){
        visitProfile * vp = new visitProfile(username);
        vp->show();
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
            QListWidgetItem* item = new QListWidgetItem(QIcon(PREVIEWS + filename), (orig).c_str());
            item->setData(Qt::UserRole, user.c_str());
            ui->listView->addItem(item);
        }
        peer.clearTempImages();
    }

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

void home::on_listView_itemClicked(QListWidgetItem *item)
{
    std::cout << "Look at this " << item->text().toUtf8().constData() << std::endl;
    string user = item->data(Qt::UserRole).toString().toUtf8().constData();
    std::cout << "From this " << user << endl;
    RequestImageDialog * pr = new RequestImageDialog(user);
    pr->show();



}

void home::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    auto requests = peer.imageRequests;
    requests.push_back(imageQuotaRequest("Fadi", "7ama.jpg", 7));
    requests.push_back(imageQuotaRequest("Eslam", "7ama2.jpg", 11));

    foreach(auto request, requests){
        string item =(string("The user ") + request.requester + string(" has requested ") + to_string(request.quota) + string(" views of your image ") + request.imageName);
        //auto item = "hi there";
//        cout << request.requester << endl;
//        cout << request.imageName << endl;
//        cout << item << endl;
        auto widg = new QListWidgetItem(item.c_str());
        widg->setData(Qt::UserRole, request.requester.c_str());
        widg->setData(Qt::DecorationRole, request.quota);
        widg->setData(Qt::DisplayPropertyRole, request.imageName.c_str());

        ui->listWidget->addItem(widg);
    }

//    QDir directory(PREVIEWS);
//    QStringList images = directory.entryList(QStringList() ,QDir::Files);
//    foreach(QString filename, images) {
//        string orig = filename.toUtf8().constData();
//        string user = removeUserfromName(orig);
//        QListWidgetItem* item = new QListWidgetItem(QIcon(PREVIEWS + filename), (orig).c_str());
//        item->setData(Qt::UserRole, user.c_str());
//        ui->listView->addItem(item);
//    }

}


void home::on_listWidget_itemClicked(QListWidgetItem *item)
{
    cout << "hi there" << endl;
    string requester = item->data(Qt::UserRole).toString().toUtf8().constData();
    string imageName = item->data(Qt::DisplayPropertyRole).toString().toUtf8().constData();
    int quota = item->data(Qt::DecorationRole).toInt();

    ApproveRequestDialog * dlg = new ApproveRequestDialog(requester, imageName, quota);
    dlg->show();



}
