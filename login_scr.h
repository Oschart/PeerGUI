#ifndef LOGIN_SCR_H
#define LOGIN_SCR_H

#include <QWidget>

namespace Ui {
class login_scr;
}

class login_scr : public QWidget
{
    Q_OBJECT

public:
    explicit login_scr(QWidget *parent = nullptr);
    ~login_scr();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::login_scr *ui;
};

#endif // LOGIN_SCR_H
