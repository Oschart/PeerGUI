#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QListWidgetItem>


namespace Ui {
class home;
}

class home : public QWidget
{
    Q_OBJECT

public:
    explicit home(QWidget *parent = nullptr);
    ~home();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_8_clicked();




    void on_pushButton_13_clicked();


    void on_pushButton_6_clicked();


    void on_pushButton_5_clicked();

    void on_listView_2_itemPressed(QListWidgetItem *item);

    void on_listView_itemClicked(QListWidgetItem *item);

    void on_pushButton_7_clicked();


    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_2_itemClicked(QListWidgetItem *item);

    void on_pushButton_4_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

private:
    Ui::home *ui;
};

#endif // HOME_H
