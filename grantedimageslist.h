#ifndef GRANTEDIMAGESLIST_H
#define GRANTEDIMAGESLIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class GrantedImagesList;
}

class GrantedImagesList : public QDialog
{
    Q_OBJECT

public:
    explicit GrantedImagesList(QWidget *parent = nullptr);
    ~GrantedImagesList();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::GrantedImagesList *ui;
};

#endif // GRANTEDIMAGESLIST_H
