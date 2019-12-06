#ifndef VISITPROFILE_H
#define VISITPROFILE_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include "globals.h"

namespace Ui {
class visitProfile;
}

class visitProfile : public QWidget
{
    Q_OBJECT

public:
    explicit visitProfile(std::string user, QWidget *parent = nullptr);
    ~visitProfile();

private slots:
    void on_listView_viewportEntered();

    void on_listView_itemClicked(QListWidgetItem *item);

private:
    Ui::visitProfile *ui;
    std::string user;
};

#endif // VISITPROFILE_H
