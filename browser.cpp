#include "browser.h"
#include "ui_browser.h"
#include <QFileSystemModel>
#include <QWidget>
#include <QListWidget>

browser::browser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::browser)
{
    ui->setupUi(this);

    //QListWidget *listView = new QListWidget;

    ui->listView->setViewMode(QListWidget::IconMode);

    ui->listView->setIconSize(QSize(50, 50));

    ui->listView->setResizeMode((QListWidget::Adjust));

    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull1.jpg"), "OSKAR"));
    ui->listView->addItem(new QListWidgetItem(QIcon("/home/wan/Downloads/imgs/skull2.jpg"), "FADI"));

    //ui->listView->show();

}

browser::~browser()
{
    delete ui;
}



void browser::on_listView_itemSelectionChanged()
{

}
