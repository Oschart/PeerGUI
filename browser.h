#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>

namespace Ui {
class browser;
}

class browser : public QWidget
{
    Q_OBJECT

public:
    explicit browser(QWidget *parent = nullptr);
    ~browser();

private slots:





    void on_listView_itemSelectionChanged();

private:
    Ui::browser *ui;
};

#endif // BROWSER_H
