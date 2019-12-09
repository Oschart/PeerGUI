#include "visitprofile.h"
#include "ui_visitprofile.h"
#include "requestimagedialog.h"

visitProfile::visitProfile(std::string user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::visitProfile), user(user)
{
    ui->setupUi(this);

    ui->listView->setViewMode(QListWidget::IconMode);

    ui->listView->setIconSize(QSize(50, 50));

    ui->listView->setResizeMode((QListWidget::Adjust));
    ui->listView->clear();

    int res =peer.getUserPreviews(user);
    if (res == -1)  {QMessageBox::information (this, "Previews", (string("Could not reach the user ") + user).c_str());}
    else  {
        QDir directory(peer.PREVIEWS.c_str());
        QStringList images = directory.entryList(QStringList() ,QDir::Files);
        foreach(QString filename, images) {
            string orig = filename.toUtf8().constData();
            string user = removeUserfromName(orig);
            if (this->user == user) {
                QListWidgetItem* item = new QListWidgetItem(QIcon(peer.PREVIEWS.c_str() + filename), (orig).c_str());
                item->setData(Qt::UserRole, user.c_str());
                ui->listView->addItem(item);
            }
        }
        cout << "I will clear temp images" << endl;
        peer.clearTempImages();
        this->show();
    }


}

visitProfile::~visitProfile()
{
    delete ui;
}

void visitProfile::on_listView_viewportEntered()
{

}

void visitProfile::on_listView_itemClicked(QListWidgetItem *item)
{
    RequestImageDialog * pr = new RequestImageDialog(user, item->text().toStdString());
    pr->show();
}
