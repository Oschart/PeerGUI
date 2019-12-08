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
    explicit ViewGrantedImageDialog(std::string fileName, QWidget *parent = nullptr);
    ~ViewGrantedImageDialog();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::ViewGrantedImageDialog *ui;
    std::string fileName;
};

#endif // VIEWGRANTEDIMAGEDIALOG_H
