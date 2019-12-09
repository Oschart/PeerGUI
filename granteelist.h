#ifndef GRANTEELIST_H
#define GRANTEELIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class GranteeList;
}

class GranteeList : public QDialog
{
    Q_OBJECT

public:
    explicit GranteeList(std::string imageName, QWidget *parent = nullptr);
    ~GranteeList();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::GranteeList *ui;
    std::string imageName;
};

#endif // GRANTEELIST_H
