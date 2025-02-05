#ifndef OFFERNEWQUOTADIALOG_H
#define OFFERNEWQUOTADIALOG_H

#include <QDialog>
using namespace std;

namespace Ui {
class OfferNewQuotaDialog;
}

class OfferNewQuotaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OfferNewQuotaDialog(string, string, QWidget *parent = nullptr);
    ~OfferNewQuotaDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::OfferNewQuotaDialog *ui;
    string user;
    string imageName;
};

#endif // OFFERNEWQUOTADIALOG_H
