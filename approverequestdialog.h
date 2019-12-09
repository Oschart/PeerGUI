#ifndef APPROVEREQUESTDIALOG_H
#define APPROVEREQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class ApproveRequestDialog;
}

class ApproveRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApproveRequestDialog(bool, std::string ,std::string, int, int, QWidget *parent = nullptr);
    ~ApproveRequestDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::ApproveRequestDialog *ui;
    std::string requester;
    std::string imageName;
    int quota;
    bool newImage;
    int index;
};

#endif // APPROVEREQUESTDIALOG_H
