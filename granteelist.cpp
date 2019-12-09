#include "granteelist.h"
#include "ui_granteelist.h"
#include "offernewquotadialog.h"
#include <vector>

GranteeList::GranteeList(std::string _img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GranteeList), imageName(_img)
{
    ui->setupUi(this);

    //assumption
    std::vector<std::string> users; std::vector<int> quotas;
    users.push_back("Oscar");
    quotas.push_back(54);

    ui->listWidget->clear();
    for(int i = 0 ; i < users.size(); i++) {
        auto item = new QListWidgetItem((users[i] + " -- " + std::to_string(quotas[i]).c_str() + " views remaining").c_str());
        item->setSizeHint(QSize(item->sizeHint().width(), 50));
        item->setData(Qt::UserRole, users[i].c_str());
        item->setData(Qt::DecorationPropertyRole, quotas[i]);
        ui->listWidget->addItem(item);
    }
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
