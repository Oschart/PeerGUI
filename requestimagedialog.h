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
    explicit RequestImageDialog(std::string owner, QWidget *parent = nullptr);
    ~RequestImageDialog();

private:
    Ui::RequestImageDialog *ui;
    std::string owner;
};

#endif // REQUESTIMAGEDIALOG_H
