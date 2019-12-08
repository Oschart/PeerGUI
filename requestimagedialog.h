#ifndef REQUESTIMAGEDIALOG_H
#define REQUESTIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class RequestImageDialog;
}

class RequestImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RequestImageDialog(std::string owner, std::string imageName, QWidget *parent = nullptr);
    ~RequestImageDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::RequestImageDialog *ui;
    std::string owner;
    std::string imageName;
};

#endif // REQUESTIMAGEDIALOG_H
