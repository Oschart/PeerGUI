#ifndef REG_SCR_H
#define REG_SCR_H

#include <QWidget>

namespace Ui {
class reg_scr;
}

class reg_scr : public QWidget
{
    Q_OBJECT

public:
    explicit reg_scr(QWidget *parent = nullptr);
    ~reg_scr();

private slots:
    void on_pushButton_clicked();

private:
    Ui::reg_scr *ui;
};

#endif // REG_SCR_H
