#ifndef VIEWGRANTEDIMAGEDIALOG_H
#define VIEWGRANTEDIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ViewGrantedImageDialog;
}

class ViewGrantedImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewGrantedImageDialog(std::string owner, std::string fileName, QWidget *parent = nullptr);
    ~ViewGrantedImageDialog();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_pushButton_clicked();

private:
    Ui::ViewGrantedImageDialog *ui;
    std::string fileName;
    std::string owner;
};

#endif // VIEWGRANTEDIMAGEDIALOG_H
