#include "granteelist.h"
#include "ui_granteelist.h"
#include "offernewquotadialog.h"
#include <vector>
#include "globals.h"

GranteeList::GranteeList(std::string _img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GranteeList), imageName(_img)
{
    ui->setupUi(this);

    auto & data = peer.receiverQuota;
    map<string,int> imageMap;
    if (data.count(imageName)){
        imageMap = data[imageName];
    }
    ui->listWidget->clear();
    for (auto i = imageMap.begin(); i != imageMap.end(); i++){

        auto item = new QListWidgetItem((i->first + " -- " + std::to_string(i->second).c_str() + " views remaining").c_str());
        item->setSizeHint(QSize(item->sizeHint().width(), 50));
        item->setData(Qt::UserRole, i->first.c_str());
        item->setData(Qt::DecorationPropertyRole, i->second);
        ui->listWidget->addItem(item);
    }


//    std::vector<std::string> users; std::vector<int> quotas;
//    users.push_back("Oscar");
//    quotas.push_back(54);

}

GranteeList::~GranteeList()
{
    delete ui;
}

void GranteeList::on_listWidget_itemClicked(QListWidgetItem *item)
{
    OfferNewQuotaDialog * onqd = new OfferNewQuotaDialog(item->data(Qt::UserRole).toString().toStdString(), imageName);
    onqd->show();
}
